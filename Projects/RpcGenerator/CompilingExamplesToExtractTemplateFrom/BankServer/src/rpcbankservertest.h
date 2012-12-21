#ifndef RPCBANKSERVERTEST_H
#define RPCBANKSERVERTEST_H

#include "objectonthreadhelper.h"
#include "rpcbankserver.h"
#include "bankserverdebugwidget.h"
#include "AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.h"

class RpcBankServerTest : public QObject
{
    Q_OBJECT
public:
    explicit RpcBankServerTest(QObject *parent = 0);
private:
    bankServerDebugWidget m_BankServerDebugWidget;
    ObjectOnThreadHelper<RpcBankServer> m_RpcBankServerThreadHelper;
    //BankServer *m_BankServer;
    ObjectOnThreadHelper<RpcBankServerClientsHelper> m_RpcBankServerClientsHelperThreadHelper;
    //RpcBankServerClientsHelper *m_RpcBankServerClientsHelper;

    //Instantiate
    inline void initializeBusinessAndClientsHelperIfReady() { if(checkBusinessAndClientsHelperAreInstantiated()) initializeBankServerAndClientsHelper(); }
    inline bool checkBusinessAndClientsHelperAreInstantiated() { return (m_RpcBankServerInstantiated && m_RpcBankServerClientsHelperInstantiated); }
    void initializeBankServerAndClientsHelper();
    bool m_RpcBankServerInstantiated;
    bool m_RpcBankServerClientsHelperInstantiated;

    //Initialize
    inline void emitInitializedIfBusinessAndClientsHelperAreInitialized() { if(checkBusinessAndClientsHelperAreInitialized()) emit initialized(); }
    inline bool checkBusinessAndClientsHelperAreInitialized() { return (m_RpcBankServerInitialized && m_RpcBankServerClientsHelperInitialized); }
    bool m_RpcBankServerInitialized;
    bool m_RpcBankServerClientsHelperInitialized;

    //connections-only pointers
    RpcBankServer *m_RpcBankServer;
    RpcBankServerClientsHelper *m_RpcBankServerClientsHelper;

    void daisyChainConnections();
signals:
    void d(const QString &);
    void initialized();

    void initializeRpcBankServerRequested(RpcBankServerClientsHelper*);
    void initializeRpcBankServerClientsHelperRequested(MultiServerAbstractionArgs);

    void startRpcBankServerRequested();
    //start clients helper daisy chains to bank server's started() signal

public slots:
    void handleRpcBankServerInstantiated();
    void handleRpcBankServerInitialized();

    void handleRpcBankServerClientsHelperIntantiated();
    void handleRpcBankServerClientsHelperInitialized();
    void handleRpcBankServerClientsHelperStarted();
};

#endif // RPCBANKSERVERTEST_H
