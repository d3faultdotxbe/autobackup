#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

#include <QVariant>

CleanRoomFrontPageDefaultViewRequestFromQt::CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoomSession *session, QObject *objectCallback, const QString &slotCallback)
    : IFrontPageDefaultViewRequest(session)
    , m_ObjectCallback(objectCallback)
{
    m_SlotCallback = slotCallback.toStdString();
}
void CleanRoomFrontPageDefaultViewRequestFromQt::regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs)
{
    QMetaObject::invokeMethod(m_ObjectCallback, m_SlotCallback.c_str(), Q_ARG(QVariantList, responseArgs));
}
