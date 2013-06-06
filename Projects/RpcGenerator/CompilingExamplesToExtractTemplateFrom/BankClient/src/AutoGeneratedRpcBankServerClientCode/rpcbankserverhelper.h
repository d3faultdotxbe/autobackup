#ifndef RPCBANKSERVERHELPER_H
#define RPCBANKSERVERHELPER_H

#include <QObject>
//#include <QThread>

#include "ibankserverclientprotocolknower.h"
#include "iacceptrpcbankserverclientactiondeliveries_and_iemitbroadcastsforsignalrelayhack.h"

#include "irpcbankserverclientbusiness.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientactiondispensers.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientbroadcastdispensers.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h"
#include "AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h"


#include "multiserverclientabstraction.h"

class RpcBankServerHelper : public IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack
{
    Q_OBJECT
public:
    explicit RpcBankServerHelper(IRpcBankServerClientBusiness *rpcBankServerClient);
    inline RpcBankServerClientActionDispensers *actionDispensers() { return m_ActionDispensers; }
private:
    inline void emitInitializedSignalIfReady() { if(checkInitializedAndAllActionDispensersClaimed()) { emit initialized(); } }
    inline bool checkInitializedAndAllActionDispensersClaimed() { return (m_Initialized && m_ActionDispensers->everyDispenserIsCreated()); }
    bool m_Initialized;


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
    void stopped();
public slots:
    void initialize(MultiServerClientAbstractionArgs multiServerClientAbstractionArgs);
    void handleBusinessDoneClaimingActionDispensersAndConnectingToBroadcastSignals();
    void start();
    void stop();
};

#endif // RPCBANKSERVERHELPER_H
