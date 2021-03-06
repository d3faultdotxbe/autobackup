//THIS FILE IS NOT GENERATED BY RPC GENERATOR. IT IS ONLY TO GET MY AUTO-DRILLDOWN STUFF WORKING ASAP IN THE COMPILING TEMPLATE SOURCE
#include "cleanroomdb.h"

#include <QScopedPointer>
#include <QMapIterator>

#include "cleanroomdoc.h"

//should this be Db::post(QByteArray data) or CleanRoomDb::postCleanRoomDoc(CleanRoomDoc doc) -- there's huge crossover CONSIDERING i want all my data to be cleanroom docs xD. really though, it doesn't make much of a difference
CleanRoomDb::CleanRoomDb(QObject *parent)
    : QObject(parent)
{ }
void CleanRoomDb::getLatestCleanRoomDocs(ICleanRoomFrontPageDefaultViewRequest *requestWeAreMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer, int numLatestDocsToGet)
{
    //int numDocsToGet = qMin(numLatestDocsToGet, m_Db.size());
    int numDocsGotSoFar = 0;
    QMapIterator<QDateTime, QByteArray> it(m_Db);
    QStringList ret;
    while(it.hasNext() && ((++numDocsGotSoFar) <= numLatestDocsToGet))
    {
        it.next();
        QScopedPointer<CleanRoomDoc> doc(CleanRoomDoc::fromJson(it.value()));
        ret.append(doc->Timestamp.toString() + " - " + doc->Username + " - " + doc->LicenseIdentifier + " - " + doc->Data);
    }
    bool lcbOpSuccess = (ret.size() == numLatestDocsToGet);
    emit getLatestCleanRoomDocsFinished(requestWeAreMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer, false, lcbOpSuccess, ret); //I could even jump the gun and call request.respond here/now! It would certainly be more efficient to do so... but fucks with my head/design too much so idfk. It would work recursively like that if it was an IRequest. IRequest makes a promise that "respond(...args...)" is thread safe. With Qt, it uses AutoConnection. With Wt, WServer::post. More kinds can/will be added. Oh look I finally found a[n extremely good] use for IRequest (as of writing, I abstract to I<Business>Request)
}
void CleanRoomDb::postCleanRoomDoc(QByteArray data)
{
    QScopedPointer<CleanRoomDoc> doc(CleanRoomDoc::fromJson(data));
    m_Db.insert(doc->Timestamp, data);
}
