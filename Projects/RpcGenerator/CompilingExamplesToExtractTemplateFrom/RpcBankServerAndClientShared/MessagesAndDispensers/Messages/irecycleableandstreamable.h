#ifndef IRECYCLEABLEANDSTREAMABLE_H
#define IRECYCLEABLEANDSTREAMABLE_H

#include <QObject>

#include "../../rpcbankserverheader.h"

class IRecycleableAndStreamable : public QObject
{
    Q_OBJECT
public:
    explicit IRecycleableAndStreamable(QObject *parent);
    virtual void streamIn(QDataStream &in)=0;
    virtual void streamOut(QDataStream &out)=0;
    RpcBankServerHeader Header; //it's a member, but we don't want to stream it because we have to stream it into a stack alloc'd header first and then retrieve the stored message from pending (it's stored by the header contents. chicken and egg problem)
    void doneWithMessage();
signals:
    void doneWithMessageSignal();
};
inline QDataStream &operator>>(QDataStream &in, IRecycleableAndStreamable &message)
{
    message.streamIn(in);
    return in;
}
inline QDataStream &operator<<(QDataStream &out, IRecycleableAndStreamable &message)
{
    message.streamOut(out);
    return out;
}

#endif // IRECYCLEABLEANDSTREAMABLE_H