#ifndef RPCBANKSERVER_H
#define RPCBANKSERVER_H

#include <QThread>

#include "objectonthreadhelper.h"
#include "AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.h"
#include "bankdbhelper.h"
#include "rpcbitcoinhelper.h"

class RpcBankServer : public QObject /* : public IRpcBankServerBusiness */ //removing that IRpcBankServerBusiness means we can represent multiple Rpc Services simultaneously? Being an Rpc Service should not make the business depend on BEING a certain type of object, because now we can't be multiple of them since they are QObjects and QObjects have a diamond inheritence problem
{
    Q_OBJECT
public:
    explicit RpcBankServer(QObject *parent = 0);
    void instructBackendObjectsToClaimRelevantBroadcastDispensers();
    void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
    void connectRpcBankServerClientActionRequestSignalsToBankServerImplSlots(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries *actionRequestSignalEmitter);
private:
    RpcBankServerClientsHelper *m_RpcBankServerClientsHelper;

    //Instantiate
    inline void initializeAllBackendsIfIAmInitializedAndAllBackendThreadsAreInstantiated() { if(m_InitializeSlotCalled && (m_BitcoinInstantiated && m_BankDbInstantiated /* && etc-for-every-backend */)) initializeAllBackends(); }
    void initializeAllBackends();
    //self
    bool m_InitializeSlotCalled;
    //all backends (that have their own threads)
    bool m_BitcoinInstantiated;
    bool m_BankDbInstantiated;
    //etc


    //Initialize
    inline void emitDoneUsingRpcBankServerClientsHelperDuringInitializationIfAllRelevantBackendsAreDoneUsingIt() { if(m_BitcoinDoneUsingRpcBankServerClientsHelper && m_BankDbDoneUsingRpcBankServerClientsHelper/*m_RpcBankServerClientsHelper->broadcastDispensers()->everyDispenserIsCreated()*/) m_DoneUsingRpcBankServerDuringInitialization = true; emit doneUsingRpcBankServerClientsHelperDuringInitialization(); }
    //etc for each rpc service

    bool m_DoneUsingRpcBankServerDuringInitialization;
    //etc for each rpc service

    bool m_BitcoinDoneUsingRpcBankServerClientsHelper;
    bool m_BankDbDoneUsingRpcBankServerClientsHelper;
    //etc for each backend for each rpc service


    //Start/Stop
    inline void emitStartedIfAllBackendsStarted() { if(m_BitcoinStarted && m_BankDbStarted /* && etc-for-each-backend */) emit started(); }
    inline void emitStoppedIfAllBackendsStopped() { if(!m_BitcoinStarted && !m_BankDbStarted /* && !etc-for-each-backend */) emit stopped(); }
    bool m_BitcoinStarted;
    bool m_BankDbStarted;
    //etc for each backend

    ObjectOnThreadHelper<BankDbHelper> m_BankDbHelperThreadHelper;
    ObjectOnThreadHelper<RpcBitcoinHelper> m_BitcoinThreadHelper;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();

    void bitcoinInitializeRequested(RpcBankServerClientsHelper*);
    void bankDbInitializeRequested(RpcBankServerClientsHelper*);
    //etc for each backend

    void doneUsingRpcBankServerClientsHelperDuringInitialization();
    //etc for each rpc service

    //using async (among 'backends' such as bitcoin/bankdbhelper) start/stop routines for now... fuck it
    void startBackendsRequested();
    void stopBackendsRequested();

    void simulatePendingBalanceDetectedBroadcastRequested();
    void simulateConfirmedBalanceDetectedBroadcastRequested();
public slots:
    void initialize(RpcBankServerClientsHelper *rpcBankServerClientsHelper /*, etc for each rpc service */);
    void start();
    void stop();

    //all 4 'etc' for each backend
    void handleBitcoinInstantiated();
    void handleBitcoinInitialized();
    void handleBitcoinStarted();
    void handleBitcoinStopped();

    void handleBankInstantiated();
    void handleBankDbInitialized();
    void handleBankDbStarted();
    void handleBankDbStopped();

    void emitInitializedIfDoneWithAllRpcServices();
};

#endif // RPCBANKSERVER_H
