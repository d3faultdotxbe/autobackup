#include "cleanroomsession.h"

#include "cleanroomnewsessionrequestfromqt.h"
#include "cleanroomnewsessionrequestfromwt.h"

#include "cleanroomfrontpagedefaultviewrequestfromqt.h"
#include "cleanroomfrontpagedefaultviewrequestfromwt.h"

void CleanRoomSession::createNewSession(CleanRoom *cleanRoom, QObject *objectToCallbackTo, const char *callbackSlot)
{
    CleanRoomNewSessionRequestFromQt *request = new CleanRoomNewSessionRequestFromQt(cleanRoom, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void CleanRoomSession::createNewSession(CleanRoom *cleanRoom, const std::string &wtSessionId, boost::function<void (CleanRoomSession*)> wApplicationCallback)
{
    CleanRoomNewSessionRequestFromWt *request = new CleanRoomNewSessionRequestFromWt(cleanRoom, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(QObject *objectToCallbackTo, const char *callbackSlot)
{
    CleanRoomFrontPageDefaultViewRequestFromQt *request = new CleanRoomFrontPageDefaultViewRequestFromQt(m_CleanRoom, objectToCallbackTo, callbackSlot);
    invokeRequest(request);
}
void CleanRoomSession::requestNewCleanRoomFrontPageDefaultView(const std::string &wtSessionId, boost::function<void (QVariantList)> wApplicationCallback)
{
    CleanRoomFrontPageDefaultViewRequestFromWt *request = new CleanRoomFrontPageDefaultViewRequestFromWt(m_CleanRoom, wtSessionId, wApplicationCallback);
    invokeRequest(request);
}
