#-------------------------------------------------
#
# Project created by QtCreator 2012-07-18T03:17:30
#
#-------------------------------------------------

QT       += core gui network

TARGET = BankServer
TEMPLATE = app


SOURCES += main.cpp\
        bankserverdebugwidget.cpp \
    bankdbhelper.cpp \
    AutoGeneratedRpcBankServerCode/ssltcpserverandbankserverprotocolknower.cpp \
    AutoGeneratedRpcBankServerCode/irpcbankserverbusiness.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Messages/Broadcasts/serverconfirmedbalancedetectedmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Messages/Broadcasts/serverpendingbalancedetectedmessage.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Broadcasts/serverconfirmedbalancedetectedmessagedispenser.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Broadcasts/serverpendingbalancedetectedmessagedispenser.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.cpp \
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.cpp \
    AutoGeneratedRpcBankServerCode/iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/iactionmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.cpp \
    rpcbankservertest.cpp \
    rpcbankserver.cpp \
    rpcbitcoinhelper.cpp \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.cpp \
    AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.cpp

HEADERS  += bankserverdebugwidget.h \
    bankdbhelper.h \
    AutoGeneratedRpcBankServerCode/ssltcpserverandbankserverprotocolknower.h \
    AutoGeneratedRpcBankServerCode/irpcbankserverbusiness.h \
    ../../RpcBankServerAndClientShared/rpcbankserverheader.h \   
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Actions/servercreatebankaccountmessagedispenser.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Actions/servergetaddfundskeymessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Messages/Broadcasts/serverconfirmedbalancedetectedmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Messages/Broadcasts/serverpendingbalancedetectedmessage.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Broadcasts/serverconfirmedbalancedetectedmessagedispenser.h \
    AutoGeneratedRpcBankServerCode/MessagesAndDispensers/Dispensers/Broadcasts/serverpendingbalancedetectedmessagedispenser.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h \
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h \
    AutoGeneratedRpcBankServerCode/iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/iactionmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h \
    rpcbankservertest.h \
    rpcbankserver.h \
    rpcbitcoinhelper.h \
    AutoGeneratedRpcBankServerCode/rpcbankserverclientshelper.h

RESOURCES += AutoGeneratedRpcBankServerCode/SslCertsAndKeys/RpcBankServerSslResources.qrc
#include(../../../../../GloballySharedClasses/ssltcpservertestcerts.pri)

include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../../GloballySharedClasses/multiserverabstraction.pri)
include(../../../../../GloballySharedClasses/ssltcpserver.pri)
