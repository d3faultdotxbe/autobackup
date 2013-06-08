#ifndef RPCBANKSERVERHELPER_H
#define RPCBANKSERVERHELPER_H

#include <QObject>
//#include <QThread>

#ifndef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
#define WE_ARE_RPC_BANK_SERVER_CLIENT_CODE //TODOreq: oh god just realized that if a single binary is both a client and a server then these ifdefs will fuck everything up probably. I guess I'd need to un-set them somewhere in order to fix that... or just change up my code and not use them altogether heh. Fuck it for now... but it is a big deal
//^^It might even make sense to see if "we are server code" is defined here and then undefine it here (and do the same but opposite in the corresponding server code). Not sure really though and that might make it depend on how the user #includes etc and cause all sorts of confusion heh...
#endif

#define RPC_BANK_SERVER_UNIQUE_CLIENT_ID 0 //TODOreq: If Rpc Generator app is used to spit out multiple "smart clients" for use on different physical machines (we have some sort of topology view/generator for our entire server heirchy etc), then it should increment this number for each one. If the user is copy/pasting the code, they should increment it. NOT incrementing it COULD be detrimental, however that would depend on both a QDateTime::currentDateTime and a qrand() call both returning back the exact same thing on two different physical machines. Incrementing this client id means that even that is safe :)...
//^^^TODOreq: wait a minute, doesn't the server keep a separate list of "message ids" per client connected? So if they do have the same message id it doesn't matter? Hmmmmmm.... I think that is actually the case now so yea my design changes have made this obsolete methinks
//^^^^^Yea bro "cookie" is essentially client id... so we already have it covered :-P
//Cookie isn't streamed with every message, fuck yea I am awesome at coding :). It's only used during hello phase. For a sec there I forgot and thought it was streamed with every message... but then I surprised myself woot. "There's a method to my madness" is so overused and usually by idiots doing non-complex things... but I think it fits rather nicely here :-P

#include "ibankserverclientprotocolknower.h"
#include "iacceptrpcbankserverclientactiondeliveries_and_iemitactionsandbroadcastsbothwithmessageasparamforsignalrelayhack.h"

#include "irpcbankserverclientbusiness.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientactiondispensers.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientbroadcastdispensers.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h"

#include "rpcbankserverclientprotocolknowerfactory.h"
#include "multiserverclientabstraction.h"

//This class is essentially a "smart client" that encapsulates many protocol "clients" (ssl client, tcp client, local client). Each protocol type can even have multiple connections to various servers. We use one of 'this' class per Rpc Service. Want another Rpc Service? Instantiate another one in the class that uses it. However TODOoptimization it makes sense to use existing connections if "by chance" (will happen quite often in practice) the rpc service uses the same server. Or maybe it doesn't matter because connecting to the same ip on a different port is ESSENTIALLY "using the same connection" like I am talking about in this optimization. I really don't know the answer to that, it's a technical network question I lack the knowledge to answer. BUT FOR NOW I AM HAPPY WITH JUST PUTTING OTHER RPC SERVICES ON OTHER PORTS. Is even a slight optimization to not use rpc service id's because then we don't have to specify them. The port takes care of that and it has to be specified anyways~
class RpcBankServerHelper : public IAcceptRpcBankServerClientActionDeliveries_AND_IEmitActionsAndBroadcastsBothWithMessageAsParamForSignalRelayHack
{
    Q_OBJECT
public:
    explicit RpcBankServerHelper(QObject *parent = 0);
    inline RpcBankServerClientActionDispensers *actionDispensers() { return m_ActionDispensers; }
private:
    MultiServerClientsAbstractionArgs m_MultiServerClientAbstractionArgs_HACK2PASS2START;
    inline void emitInitializedSignalIfReady() { if(checkInitializedAndAllActionDispensersClaimed()) { emit initialized(); } }
    inline bool checkInitializedAndAllActionDispensersClaimed() { return (m_Initialized && m_ActionDispensers->everyDispenserIsCreated()); }
    bool m_Initialized;

    RpcBankServerClientActionDispensers *m_ActionDispensers;

    RpcBankServerClientProtocolKnowerFactory m_RpcBankServerClientProtocolKnowerFactory;
    MultiServerClientAbstraction m_MultiServerClientAbstraction;
#if 0
    QThread *m_BusinessThread;
    IRpcBankServerClientBusiness *m_RpcBankServerClient;
    RpcBankServerClientActionDispensers *m_ActionDispensers;
    RpcBankServerClientBroadcastDispensers *m_BroadcastDispensers;
    QThread *m_TransporterThread;
    IBankServerClientProtocolKnower *m_Transporter;

    void moveTransporterToItsOwnThreadAndStartTheThread();
    void moveBusinessToItsOwnThreadAndStartTheThread();

    void daisyChainInitStartStopConnections();
#endif
signals:
    void d(const QString &);
    void initialized();
    void started();
    void readyForActionRequests();
    void stopped();
public slots:
    void initialize(MultiServerClientsAbstractionArgs multiServerClientAbstractionArgs);
    void handleBusinessDoneClaimingActionDispensersAndConnectingToBroadcastSignals();
    void start();

    //void addAndInitializeAndStartSslConnection(SslTcpClientArgs sslTcpClientArgs);
    //TODOreq: add these helpers and implement them and also for the other 2 protocols

    //void stopAndRemoveAndDeleteSslConnection(somePointerToItSomehowWtf);
    //TODOreq: ditto

    void createBankAccountDelivery();
    void getAddFundsKeyDelivery();

    void stop();
};

#endif // RPCBANKSERVERHELPER_H
