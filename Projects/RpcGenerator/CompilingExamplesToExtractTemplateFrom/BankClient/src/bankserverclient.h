#ifndef BANKSERVERCLIENT_H
#define BANKSERVERCLIENT_H

#include "AutoGeneratedRpcBankServerClientCode/rpcbankserverhelper.h"
//#include "AutoGeneratedRpcBankServerClientCode/irpcbankserverclientbusiness.h" //TODOreq: merge and delete because obsolete

//had renamed this to RpcBankServerClient but then realized that it is the only class in all of the classes for these specs that is "100% non-rpc related and user code". Meaning it could be using multiple Rpc Server Helpers. I came about this when realizing that the "handleRpcServerHelperInstantiated" should NOT be called "initialize". We have our own initialize that may or may not (DOES IN THIS CASE BUT MAY INCLUDE MULTIPLE OR NONE IN OTHER CASES) have anything to do with Rpc anything.
class BankServerClient /*: public IRpcBankServerClientBusiness*/ : public QObject
{
    Q_OBJECT
public:
    BankServerClient(QObject *parent = 0);
    //void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
    //void connectRpcBankServerSignalsToBankServerClientImplSlots(IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam *signalEmitter);
private:
    //these dispensers will be in backend objects once i think of some..
    ClientCreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    ClientGetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;
signals:
    void d(const QString &);
    void initialized();

    void beginStartProcedureRequested();
    void started();

    void simulateCreateBankAccountActionRequested();
    void simulateGetAddFundsKeyActionRequested();

    void beginStoppingProcedureRequested();
    void stopped();
public slots:
    void initialize(RpcBankServerHelper *rpcBankServerHelper);
    void start();
    void handleRpcBankServerHelperReadyForActionRequests();
    void beginStoppingProcedure();
    void finishStoppingProcedure();

    //debug
    void simulateCreateBankAccountAction(QString username);
    void simulateGetAddFundsKeyAction(QString username);
private slots:
    //Actions Responses
    void handleCreateBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage);
    void handleGetAddFundsKeyCompleted(GetAddFundsKeyMessage *getAddFundsKeyMessage);

#if 0
    //Action Errors
    void handleCreateBankAccountFailedUsernameAlreadyExists(CreateBankAccountMessage *createBankAccountMessage);
    void handleCreateBankAccountFailedPersistError(CreateBankAccountMessage *createBankAccountMessage);
    void handleGetAddFundsKeyFailedUsernameDoesntExist(GetAddFundsKeyMessage *getAddFundsKeyMessage);
    void handleGetAddFundsKeyFailedUseExistingKeyFirst(GetAddFundsKeyMessage *getAddFundsKeyMessage);
    void handleGetAddFundsKeyFailedWaitForPendingToBeConfirmed(GetAddFundsKeyMessage *getAddFundsKeyMessage);
#endif

    //Broadcasts
    void handlePendingBalanceDetected(ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage);
    void handleConfirmedBalanceDetected(ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage);
};

#endif // BANKSERVERCLIENT_H
