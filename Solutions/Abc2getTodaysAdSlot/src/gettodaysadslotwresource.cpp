#include "gettodaysadslotwresource.h"

#include <Wt/Http/Response>
#include <Wt/WDateTime>

#include <boost/thread/lock_guard.hpp>

#include "synchronoustodaysadslotgetter.h"

mutex GetTodaysAdSlotWResource::m_TodaysAdSlotMutex;
string GetTodaysAdSlotWResource::m_TodaysAdSlotJson;
long long GetTodaysAdSlotWResource::m_TodaysAdSlotExpirationDate = 0;

GetTodaysAdSlotWResource::GetTodaysAdSlotWResource(WObject *parent)
    : WResource(parent)
{ }
GetTodaysAdSlotWResource::~GetTodaysAdSlotWResource()
{
    beingDeleted();
}
void GetTodaysAdSlotWResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    (void)request;
    //TODOreq: something <3

    lock_guard<mutex> scopedLock(m_TodaysAdSlotMutex); //handleRequest is concurrent, but to KISS I am making it not :-P. could maybe do a "try_exlusive_lock" followed by a "blocking shared lock" (so that whoever gets exclusive lock FIRST, does updating... and the rest wait (but then can read all simultaneously when exlusive updater is done)

    long long currentDateTime = static_cast<long long>(WDateTime::currentDateTime().toTime_t());
    if(currentDateTime > (m_TodaysAdSlotExpirationDate-31)) //TODOoptional: might be better to have the http client send over what time range he wants, but too lazy to read the request for now...
    {
        //current expires in ~30 seconds, so we really want tomorrows :-P
        getTodaysAdSlot("d3fault", "0", currentDateTime); //TODOoptional: future proofing could read these values from the request...
    }

    response.setContentLength(m_TodaysAdSlotJson.length());
    response.setMimeType("application/json");
    response.out() << m_TodaysAdSlotJson;
}
//I don't know if I'm going to have the constructor call this or maybe a timer or what, but I do know I'll call it at some point, so it's my starting point. Here's what it does: gets the "current slot cache", but if it doesn't exist then it just crawls up the slots incrementing 1 at a time until it either finds one where the date range is TODAY, or it finds a non-existent slot (in which case, we... I guess... return a placeholder ("no ad") that is valid for like 5 mins or so? 5 mins to serve as a reasonable max before a purchase will show up, etc idfk). In both cases, we set up the slot cache to optimize our own next run. I am thinking that this resource will own a mutex that it will lock before calling this method, or something.
void GetTodaysAdSlotWResource::getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime)
{
    //TO DOnereq: need a shared lock thing around handleRequest (which becomes exclusive if it needs to). Since only the amount of HackyVideoBullshitSite nodes will ever be requesting this resource simultanesouly, it isn't a big deal that it's not the most optimal solution. Even if I just use exclusive locking, that's probably fine (and possibly easier to code, since the shareds might never get released or some such)
    //TODOoptional: ^^future proofing relating to that, if we specify a "campaign owner + index" in the request, we can use the combination thereof as a key into a hash of mutexes. Modifying that hash itself (adding campaigns) still poses problems with locking, but eh fuck it for now
    SynchronousTodaysAdSlotGetter getter; //scoped var because no need to keep the couchbase object/connections open for 23 hours of the day, nah mean. TODOreq: before it goes out of scope, we should save as a member the ad for the day and also when it expires (we cache it (hmm actually not sure that's a good idea seeing as it'll likely only be requested once or twice (or N where N is amount of 'users' of abc2, but there's so many options here in distributing it amongst those N in a more efficient manner (to each other) it blah)))
    if(getter.tryToGetTodaysAdSlot(campaignOwnerUsername, campaignIndex, currentDateTime))
    {
        m_TodaysAdSlotJson = getter.todaysAdSlotJson();
        m_TodaysAdSlotExpirationDate = getter.todaysAdSlotExpirationDate();
    }
    else
    {
        //make expire time 5 mins in future, otherwise every HackyBullshitVideoSite node will do the couchbase db hit all in a wonderfully inefficient manner (spike)
        m_TodaysAdSlotJson = JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER;
        m_TodaysAdSlotExpirationDate = (currentDateTime + ((5*60)-30)); //don't ask again for 4.5 mins (enforced on both client (the http request) and server (the couchbase db hit)). they are both hardcoded. server to 4.5 mins, client to 5 mins
    }
}
