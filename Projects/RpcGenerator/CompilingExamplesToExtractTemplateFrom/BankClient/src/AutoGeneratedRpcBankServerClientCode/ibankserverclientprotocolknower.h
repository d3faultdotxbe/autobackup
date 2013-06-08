#if 0
#ifndef IBANKSERVERCLIENTPROTOCOLKNOWER_H
#define IBANKSERVERCLIENTPROTOCOLKNOWER_H

#include <QObject>
#include <QHash>

#include "AutoGeneratedRpcBankServerClientCode/iemitrpcbankserverbroadcastandactionresponsesignalswithmessageasparam.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientbroadcastdispensers.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h"

class IBankServerClientProtocolKnower : public IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam
{
    Q_OBJECT
public:
    explicit IBankServerClientProtocolKnower(QObject *parent = 0);
    void setBroadcastDispensers(RpcBankServerClientBroadcastDispensers *rpcBankServerBroadcastDispensers);
    void takeOwnershipOfBroadcastsAndSetupDelivery();
private:
    //uint m_MessageIdCounter;
    //uint getUniqueMessageId();
    RpcBankServerClientBroadcastDispensers *m_RpcBankServerBroadcastDispensers;
#if 0
    too lazy to re-read these comments but i did change from list to hash as predicted

    //TODOreq: do i need pending hashes here like in IBankServerProtocolKnower on the server? i think i will, but i can't figure out how ima use them yet. they aren't used the same, that's for sure
    //eh there is only one server (by definition of server), so we don't need a hash for the pendings, just a list
    QList<CreateBankAccountMessage*> m_PendingCreateBankAccountMessagePointers;
    QList<GetAddFundsKeyMessage*> m_PendingGetAddFundsKeyMessagePointers;
    //TODOreq: i want to have a 'ok we already are working on a request that is similar/conflicts' detection, definitely. my instincts say the easiest solution is to just delete teh conflicts, but my optimizer says i should keep the detected conflicts around in case the one that beat them to it fails for some reason... in which case we can then process the next one (that we would have deleteted but saved instead)
    //--------ANYWAYS--------, after reading the above, the problem that i just now am seeing: what do i do if on the rpc client side i detect a conflict? do i still send it to rpc server? having a pending list on both the rpc client and rpc server actually sounds like a shit idea because then there's mass synchronization issues. that's the point of this comment, to note that i have to decide WHERE to keep the list. and if i decide on the server, then there's no point of having these Pending* QLists... because the server will do conflict resolution for me. But it's also less optimized... but not much, probably worth it. Sure beats some other crazy ass client/server message synchronization method (off the top of my head: record timestamps for when they were received and keep them wherever they were detected. when a message fails, get all the 'pendings' from each client and then process them in order of timestamp. this has the downside that we need to broadcast the message result (were we going to do that anyways? i think not but am not sure) in order for the client's to clear their pending request cache. or they could just time out? complicated solution, no doubt)
#endif
    QHash<quint32 /*HeaderMessageId*/, CreateBankAccountMessage*> m_PendingCreateBankAccountMessagesById;
    QHash<quint32, GetAddFundsKeyMessage*> m_PendingGetAddFundsKeyMessagesById;
protected:
    ClientPendingBalanceDetectedMessageDispenser *m_PendingBalanceDetectedMessageDispenser;
    ClientConfirmedBalanceDetectedMessageDispenser *m_ConfirmedBalanceDetectedMessageDispenser;

    virtual void myTransmit(IMessage *message)=0;

    //TODOoptimization: perhaps inline these getPending*MessageById?? they are definitely called a lot... and it really only accomplishes private/protected abstraction
    CreateBankAccountMessage *getPendingCreateBankAccountMessageById(quint32 messageId);
    GetAddFundsKeyMessage *getPendingGetAddFundsKeyMessageById(quint32 messageId);
#if 0
    void processCreateBankAccountResponseReceived(CreateBankAccountMessage *createBankAccountMessage);
    void processGetAddFundsKeyResponseReceived(GetAddFundsKeyMessage *getAddFundsKeyMessage);
#endif
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
    //outgoing Action requests
    void createBankAccountDelivery();
    void getAddFundsKeyDelivery();
};

#endif // IBANKSERVERCLIENTPROTOCOLKNOWER_H
#endif
