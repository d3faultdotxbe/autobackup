#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>
#include <QDataStream>

#include "irecycleableandstreamable.h"

class IMessage : public IRecycleableAndStreamable
{
    Q_OBJECT
public:
    explicit IMessage(QObject *owner);
    void deliver();
    void fail(quint32 failedReasonEnum);
    quint32 FailedReasonEnum;
signals:
    void deliverSignal();
};

#endif // IMESSAGE_H
