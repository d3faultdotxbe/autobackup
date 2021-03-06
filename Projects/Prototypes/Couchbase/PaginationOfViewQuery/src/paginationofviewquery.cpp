#include "paginationofviewquery.h"

#include <QScopedPointer>
#include <QDebug>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>

using namespace boost::property_tree;

#define PaginationOfViewQuery_ITEMS_PER_PAGE 2

#define APPEND_INEFFICIENT_SKIP_TO_VIEW_PATH_PLX_MACRO \
int skip = (pageNum_WithOneBeingTheFirstPage-1)*PaginationOfViewQuery_ITEMS_PER_PAGE; \
if(skip > 0) \
{ \
    viewPath += "&skip="; \
    viewPath += QString::number(skip).toStdString(); \
}

class PaginationOfViewQueryCouchbaseCookieType
{
public:
    explicit PaginationOfViewQueryCouchbaseCookieType(PaginationOfViewQuery *pointerToPaginationOfViewQuery, int pageNumberRequested)
        : PointerToPaginationOfViewQuery(pointerToPaginationOfViewQuery)
        , PageNumberRequested(pageNumberRequested)
    { }

    PaginationOfViewQuery *PointerToPaginationOfViewQuery;
    int PageNumberRequested;
};

PaginationOfViewQuery::PaginationOfViewQuery(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
{ }
void PaginationOfViewQuery::errorOutput(const string &errorString)
{
    qDebug() << QString::fromStdString(errorString);
    emit quitRequested();
}
void PaginationOfViewQuery::viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    QScopedPointer<PaginationOfViewQueryCouchbaseCookieType> theCookie(const_cast<PaginationOfViewQueryCouchbaseCookieType*>(static_cast<const PaginationOfViewQueryCouchbaseCookieType*>(cookie)));
    theCookie->PointerToPaginationOfViewQuery->viewQueryCompleteCallback(theCookie->PageNumberRequested, error, resp);
}
void PaginationOfViewQuery::viewQueryCompleteCallback(int pageNum_WithOneBeingTheFirstPage, lcb_error_t error, const lcb_http_resp_t *resp)
{
    //we pre-format the page/cache before giving to front-end. Is both memory and cpu optimization, memory because it's pointless to store a bunch of "null" value [strings], cpu because front-end would have to convert it on each request (cache hit) anyways. We cache that pre-formatted structure ofc

    qDebug() << "httpCompleteCallback";
    ViewQueryPageContentsType ret;
    if(error != LCB_SUCCESS)
    {
        qDebug() << "httpCompleteCallback has error";
        emit finishedQueryingPageOfView(ret, true);
        return;
    }
    QByteArray pathByteArray(resp->v.v0.path, resp->v.v0.npath);
    QString pathString(pathByteArray);
    qDebug() << pathString;
    QByteArray jsonPageContentsByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonPageContentsString(jsonPageContentsByteArray);
    qDebug() << jsonPageContentsString;

    ptree pt;
    std::string jsonPageContentsStdString = jsonPageContentsString.toStdString();
    std::istringstream is(jsonPageContentsStdString);
    read_json(is, pt);
    boost::optional<std::string> errorMaybe = pt.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        qDebug() << "json had error set";
        emit finishedQueryingPageOfView(ret, true);
        return;
    }
    const ptree &rowsPt = pt.get_child("rows");
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        ret.append(row.second.get<std::string>("key" /* emitted view key, username in this case*/));
    }
    emit finishedQueryingPageOfView(ret);
    if(ret.isEmpty())
    {
        return; //the below call to back() would segfault otherwise
    }

    //cache the last username's docid and all the usernames on that page!
    m_CachedPagesAndTheirLastDocIdsAndLastKeys.insert(pageNum_WithOneBeingTheFirstPage, qMakePair(rowsPt.back().second.get<std::string>("id" /* docid, that is */), ret));

    //since we inserted a cache item, update/set "total_rows" (total pages actually, based on total_rows ofc). It probably didn't change but may have, and it's cheap to just set it instead of check if it's changed and blah blah
    //m_TotalRowsInEntireView_OnlyValidWhenCacheIsNotEmpty = boost::lexical_cast<int>(pt.get<std::string>("total_rows"));
    int totalRows = boost::lexical_cast<int>(pt.get<std::string>("total_rows"));
    m_TotalPageCount_OnlyValidWhenCacheIsNotEmpty = (totalRows / PaginationOfViewQuery_ITEMS_PER_PAGE); //optimization to calculate this now rather than on every request
    if((totalRows % PaginationOfViewQuery_ITEMS_PER_PAGE) != 0) //account for the very last page not necessarily being full
    {
        ++m_TotalPageCount_OnlyValidWhenCacheIsNotEmpty;
    }
    //TODOreq: I'm not sure if I should cache the last docid [for using as startkey_docid] if the page doesn't contain the max amount of 'items per page'. For example on the very last page, only 5 of the 10 items might be shown, so uhhhh (mind=exploded). Idk tbh, but yea figure it out...
}
void PaginationOfViewQuery::initializePaginationOfViewQuery()
{
    if(!connectToCouchbase())
    {
        emit paginationOfViewQueryInitialized(false);
        return;
    }
    lcb_set_http_complete_callback(m_Couchbase, PaginationOfViewQuery::viewQueryCompleteCallbackStatic);
    emit paginationOfViewQueryInitialized(true);

    //kind of a hack, so that when the gui loads for the first time we get the first page right away
    queryPageOfView(1);
}
void PaginationOfViewQuery::queryPageOfView(int pageNum_WithOneBeingTheFirstPage) //TODOreq: if a page is queried a second time (different user) before the http callback completes, we should just append that user to the "list of users to give the http callback results to" instead of queryign it again/simultaneously lol. The same hack was used in Abc2's get+subscribe cache. Damn this prototype is getting complex as fuck...
{
    //TODOreq: pageNum_WithOneBeingTheFirstPage must be sanitized [by frontend ideally] to be greater than zero before being given to us (this note is for the Abc2 impl, as this prototype already does it)


    //re: startkey_docid and caching view queries. the "previous page" (used to determine startkey_docid) must be in the cache to use that feature, but the "next page" clearly isn't (otherwise we return it duh). Err my point is that maybe the db has changed so that starkey_docid isn't pointing to the right page "SYNC" point. Shit even hypothetically (but not in Abc2?), the startkey_docid could have been deleted... and then what? Of course it's better to just put deleted=true than to delete it for realz (db 101)... I don't really know what I'm getting at here... just thinking out loud. Hmm, what if the previous page is in the cache but is expired? Should we still use it's startkey_docid? Should we use the "closest" startkey_docid that isn't expired(cache-wise), even if it's from 5 'pages' previous? Kind of makes sense to do that. How/when should I expire the cached view queries/pages? Lazily is most efficient CPU-wise, but sheeeit has the potential to eat up assloads of RAM. QCache jumps out at me ofc, but Abc2 for some reason utterly refuses to use Qt (why? I keep asking him but he won't tell me).
    //Awww shit does startkey_docid even apply when reduce is in the equation? Methinks not, fml. If I did a "all ad campaigns by all users" (or even just "all users") thing that used pagination, then I could use startkey_docid (since there'd be no reduce), but man I really want "all users with at least one ad campaign" :(, and I don't know how to do it without a reduce :(...

    //RANDOM/semi-OT: Since my ad campaign indexes increment, my VIEW could only emit the 0th one and then blamo no need to reduce!!! This breaks if/when I enable ad campaign deleting via delete=0, (NOPE: but I could account for that by emitting the first ad campaign index seen [oh wait nvm there's no way to know whether or not]...). I think this is a fine solution for now :-/

    std::string viewPath = "_design/dev_AllUsersWithAtLeastOneAdCampaign/_view/AllUsersWithAtLeastOneAdCampaign?stale=false&limit=";
    viewPath += QString::number(PaginationOfViewQuery_ITEMS_PER_PAGE).toStdString(); //limit

    if(!m_CachedPagesAndTheirLastDocIdsAndLastKeys.isEmpty()) //our cache must not be empty lol
    {
        //check if the pageNum requested could even exist based on what total_rows was set at the last time a page was queried. If it can't, then just emit an empty page to signify "page not found"
        if(pageNum_WithOneBeingTheFirstPage > m_TotalPageCount_OnlyValidWhenCacheIsNotEmpty)
        {
            //page does not exist
            emit finishedQueryingPageOfView(ViewQueryPageContentsType());
            return;
        }


        /*
        PSEUDO:
        if previous page is in cache and not expired
        {
            use previous page's last docid as startkey_docid and last key as startkey, then do skip=1 --- and that's about it.... but heh the pseudo is much simpler xD
        }
        */

        //I have a few choices with how to expire cache items. I think I should be proactive about it since far back pages (and there could be lots of them) may be access infrequently and therefore we might run out of memory if we aren't proactive, which means I DON'T need to check expirations here/now: if it's in the cache, it isn't expired. The "cache expiry timer" timeout should be (CACHE_LENGTH / 2) btw TODOreq, which yea can be still inaccurate by ~(CACHE_LENGTH + (CACHE_LENGTH / 2)), but whatever
        //this is all a complete fucking waste of time if Abc2 never allows the public to create ad campaigns btw :-), since there'd just be "d3fault" (which I should just hardcode at that point) xD xD... but yea I mean I guess I'm still learning about views/reduces/etc in a hands on manner...

        //try to get the page about to be requested. If it's there then we have a cache CONTENT hit and don't even need to query the view :-P
        QMap<CACHED_VIEW_QUERY_PAGES_HASH_KEY_AND_VALUE_TYPE>::const_iterator it = m_CachedPagesAndTheirLastDocIdsAndLastKeys.lowerBound(pageNum_WithOneBeingTheFirstPage);
        if(it.key() == pageNum_WithOneBeingTheFirstPage)
        {
            qDebug() << "cache CONTENT hit";
            //emit immediately
            emit finishedQueryingPageOfView(it.value().second);
            return;
        }
        //cache miss, so decrement the iterator (as long as it isn't begin()) in order to get "a page before the one we want that is in the cache", and then use it's startkey_docid to get us as close as possible to the page being requested in a much more efficient than specifying startkey or skip only
        if(it == m_CachedPagesAndTheirLastDocIdsAndLastKeys.cbegin())
        {
            //cbegin() points to a page after the page we want, so use regular inefficient skip strategy
            APPEND_INEFFICIENT_SKIP_TO_VIEW_PATH_PLX_MACRO
        }
        else
        {
            //iterator points to a page greater than our page requested, but we know there's another cached page before it (simply decrement the iterator and blamo). however we need to first check we're not requesting the first page, because there is definitely not a [previous] page 0 in cache (since we start at page 1), and also the optimization doesn't even apply for the first page anyways lol (nothing to skip)
            if(pageNum_WithOneBeingTheFirstPage > 1)
            {
                --it;
                //now iterator points to "a cached page that is before (less than) the page being requested", and we know what page it is from it.key(), so we calculate how much there is left to skip based on that it.key() in addition to our constant items per page
                qDebug() << "Using startkey_docid from page:" << it.key() << "for a more efficient lookup";
                int skip = 1; //we always want to skip at least 1, because startkey/startkey_docid the last entry of that [previous] page

                //now skip entire pages worth of items if needed. this is a combination of the "inefficient skipping" _AND_ the efficient use of startkey_docid. Hell I didn't even see it described in couchbase docs, *sucks own dick*
                int numFullPagesToSkip = (pageNum_WithOneBeingTheFirstPage-it.key())-1; //the minus one is because we don't need to skip the page it points to (that was already done by specifying startkey_docid/startkey)
                skip += (numFullPagesToSkip*PaginationOfViewQuery_ITEMS_PER_PAGE); //TODOreq: verify no off by one errors, my head kind of hurts right now, but I think this is correct :-D....

                viewPath += "&starkey_docid=\"" + it.value().first + "\"&startkey=\"" + it.value().second.last() + "\"&skip=" + QString::number(skip).toStdString(); //damn this line is so sex
            }
            //else: page 1, so no skip param specified
        }

#if 0 //old hash based cache, page-1 had to exist in cache otherwise inefficient skipping was used. some of the comments may still apply
        CACHED_VIEW_QUERY_PAGES_HASH_VALUE_TYPE previousPageInCacheMaybe = m_CachedPagesAndTheirLastDocIdsAndLastKeys.value(pageNum_WithOneBeingTheFirstPage-1);
        if(previousPageInCacheMaybe.first != "" /*checking instead if second is not empty would work just as well*/)
        {
            qDebug() << "previous page existed, so we're using it's last docid as our startkey_docid";
            //previous page is in cache, so we can get the page requested pretty efficiently. TODOmb: use a map instead of a hash, and then get simply the "previous map entry" (since sorted by page, we know it's BEFORE the page we are requesting)... which may or may not be exactly page-1. But it would still be an optimization to get page-2 or page-3 if, say, we're requesting page 999999. KISS and just a hash requiring page-1 for now (otherwise optimization isn't used (when the optimization isn't used (regardless of hash or map), we fall back to the basic skip+limit strategy duh)). Hash is more efficient to lookup in, map allows us to easily get the "last page in cache before the page we want" easier (getting it in qhash would require iterating/decrementing/checking-if-exists, yuck) -- they both have tradeoffs idfk
            //Eh with more research, I am having trouble figuring out how to jump into a map for "any key less than this key x" O_o... oh shit actually maybe lowerBound does it, idfk readan. Also note that begin() is not necessarily the first page (even though it probably is).
            //TODOoptimization: if using the above described "map" strategy, we could amplify the "skip" by however much we need to. If page-2 is cached, then we skip over page-1 (even though page-1 isn't cached) simply because we know how many are on page-1. In fact, this optimization might be necessary idfk
            viewPath += "&starkey_docid=\"" + previousPageInCacheMaybe.first + "\"&startkey=\"" + previousPageInCacheMaybe.second + "\"&skip=1";
        }
        else
        {
            //regular inefficient skip
            viewPath += "&skip=";
            viewPath += QString::number((pageNum_WithOneBeingTheFirstPage-1)*itemsPerPage).toStdString(); //skip
        }
#endif
    }
    else
    {
        //cache is empty, use inefficient skip
        APPEND_INEFFICIENT_SKIP_TO_VIEW_PATH_PLX_MACRO
    }

    lcb_http_cmd_t queryPageOfViewCmd;
    queryPageOfViewCmd.version = 0;
    queryPageOfViewCmd.v.v0.path = viewPath.c_str();
    queryPageOfViewCmd.v.v0.npath = viewPath.length();
    queryPageOfViewCmd.v.v0.body = NULL;
    queryPageOfViewCmd.v.v0.nbody = 0;
    queryPageOfViewCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    queryPageOfViewCmd.v.v0.chunked = 0;
    queryPageOfViewCmd.v.v0.content_type = "application/json";
    lcb_error_t error = lcb_make_http_request(m_Couchbase, new PaginationOfViewQueryCouchbaseCookieType(this, pageNum_WithOneBeingTheFirstPage), LCB_HTTP_TYPE_VIEW, &queryPageOfViewCmd, NULL);
    //libcouchbase_make_couch_request(instance, NULL, path, npath, NULL, 0, LBCOUCHBAESE_HTTP_METHOD_GET, 1);
    if (error != LCB_SUCCESS)
    {
        qDebug() <<  "Failed to query view" << QString::fromStdString(lcb_strerror(m_Couchbase, error));
        emit quitRequested();
        return;
    }
    lcb_wait(m_Couchbase);
}
