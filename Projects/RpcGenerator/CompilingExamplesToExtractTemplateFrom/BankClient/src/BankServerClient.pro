#-------------------------------------------------
#
# Project created by QtCreator 2012-07-21T23:39:07
#
#-------------------------------------------------

QT       += core gui network

TARGET = BankServerClient
TEMPLATE = app


SOURCES += main.cpp\
    AutoGeneratedRpcBankServerClientCode/irpcbankserverclientbusiness.cpp \    
    AutoGeneratedRpcBankServerClientCode/rpcbankserverhelper.cpp \
    AutoGeneratedRpcBankServerClientCode/ibankserverclientprotocolknower.cpp \
    AutoGeneratedRpcBankServerClientCode/iemitrpcbankserverbroadcastandactionresponsesignalswithmessageasparam.cpp \
    AutoGeneratedRpcBankServerClientCode/ssltcpclientandbankserverprotocolknower.cpp \    
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.cpp \
    ../../../../../GloballySharedClasses/network/ssl/ssltcpclient.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.cpp \
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractionnonerrordeliveries.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Broadcasts/clientpendingbalancedetectedmessage.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Broadcasts/clientconfirmedbalancedetectedmessage.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Actions/clientcreatebankaccountmessage.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Actions/clientgetaddfundskeymessage.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Actions/clientcreatebankaccountmessagedispenser.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Actions/clientgetaddfundskeymessagedispenser.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientactiondispensers.cpp \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientbroadcastdispensers.cpp \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.cpp \
    rpcbankserverclienttest.cpp \
    bankserverclientdebugwidget.cpp \
    bankserverclient.cpp \
    AutoGeneratedRpcBankServerClientCode/iacceptrpcbankserverclientactiondeliveries_and_iemitbroadcastsforsignalrelayhack.cpp

HEADERS  += \
    AutoGeneratedRpcBankServerClientCode/irpcbankserverclientbusiness.h \
    AutoGeneratedRpcBankServerClientCode/rpcbankserverhelper.h \
    AutoGeneratedRpcBankServerClientCode/ibankserverclientprotocolknower.h \
    AutoGeneratedRpcBankServerClientCode/iemitrpcbankserverbroadcastandactionresponsesignalswithmessageasparam.h \
    AutoGeneratedRpcBankServerClientCode/ssltcpclientandbankserverprotocolknower.h \    
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h \    
    ../../RpcBankServerAndClientShared/rpcbankserverheader.h \
    ../../../../../GloballySharedClasses/network/ssl/ssltcpclient.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/irecycleableandstreamable.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessagebase.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h \
    ../../RpcBankServerAndClientShared/iacceptrpcbankserveractionnonerrordeliveries.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Broadcasts/clientpendingbalancedetectedmessage.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Broadcasts/clientconfirmedbalancedetectedmessage.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientpendingbalancedetectedmessagedispenser.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Broadcasts/clientconfirmedbalancedetectedmessagedispenser.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Actions/clientcreatebankaccountmessage.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Messages/Actions/clientgetaddfundskeymessage.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Actions/clientcreatebankaccountmessagedispenser.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/Actions/clientgetaddfundskeymessagedispenser.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientactiondispensers.h \
    AutoGeneratedRpcBankServerClientCode/MessagesAndDispensers/Dispensers/rpcbankserverclientbroadcastdispensers.h \
    ../../RpcBankServerAndClientShared/MessagesAndDispensers/Dispensers/irecycleabledispenser.h \
    rpcbankserverclienttest.h \
    bankserverclientdebugwidget.h \
    bankserverclient.h \
    AutoGeneratedRpcBankServerClientCode/iacceptrpcbankserverclientactiondeliveries_and_iemitbroadcastsforsignalrelayhack.h

INCLUDEPATH += ../../../../../GloballySharedClasses/

RESOURCES += AutoGeneratedRpcBankServerClientCode/SslCertsAndKeys/RpcBankServerClientSslResources.qrc


include(../../../../../GloballySharedClasses/objectonthreadhelper.pri)
include(../../../../../GloballySharedClasses/multiserverclientabstraction.pri)
