#ifndef BANKSERVER_H
#define BANKSERVER_H

#define RPC_BANK_SERVER_CODE // we define this because we _ARE_ the rpc server. this makes us parse our messages correctly using QDataStream

#include <QThread>

#include "AutoGeneratedRpcBankServerCode/irpcbankserverbusiness.h"

#include "bankdbhelper.h"
#include "bitcoinhelper.h"

class BankServer : public IRpcBankServerBusiness
{
    Q_OBJECT
public:
    explicit BankServer();
    void instructBackendObjectsToClaimRelevantDispensers();
    void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads();
private:
    BankDbHelper *m_BankDbHelper;
    QThread *m_BankDbHelperThread;

    BitcoinHelper *m_Bitcoin;
    QThread *m_BitcoinThread;

    void actualRpcConnections();
    void daisyChainInitStartStopConnections();
public slots:
    void init();
    void start();
    void stop();

    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);
signals:
    void createBankAccountRequested(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKeyRequested(GetAddFundsKeyMessage *getAddFundsKeyMessage);
};

#endif // BANKSERVER_H
