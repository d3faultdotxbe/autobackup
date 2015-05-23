#ifndef ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H
#define ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H

#include <QObject>
#include "icleanroomrequest.h"

#include "cleanroom.h"

class ICleanRoomFrontPageDefaultViewRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    ICleanRoomFrontPageDefaultViewRequest(CleanRoom *cleanRoom, double someArg0, QObject *parent = 0)
        : QObject(parent)
        , m_SomeArg0(someArg0)
    {
        connect(this, &ICleanRoomFrontPageDefaultViewRequest::frontPageDefaultViewRequested, cleanRoom, &CleanRoom::getFrontPageDefaultView);
    }
    void processRequest();
    virtual void respond(QStringList frontPageDocs)=0;
private:
    double m_SomeArg0;
signals:
    void frontPageDefaultViewRequested(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
};

#endif // ICLEANROOMFRONTPAGEDEFAULTVIEWREQUEST_H