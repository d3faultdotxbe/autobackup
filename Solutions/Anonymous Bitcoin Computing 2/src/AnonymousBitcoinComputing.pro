#This time KISS. I want the last two years of my 20s back :-/... holy shit I am (this.timeLeft() == indefinite ? "smart" : "stupid")...
#Alpha 0.1, only 332 req To Do items xD. 259 now after one ctrl+f pass, most of them seem dumb. 104 now after 2nd ctrl+f pass and let's call it Beta 0.5 <3

###############DEPLOY############
#1) Copy /usr/share/Wt/resources/ to the directory that "wtAppHere" is located in
#2) In /etc/wt/wt_config.xml, set:
#   2a) <ajax-puzzle>true</ajax-puzzle>
#   2b) <max-request-size>175</max-request-size> .... 175 kb corresponding to 576x96 24-bit uncompressed BMP ad slot filler, which comes out to 163.x kb max, but I am not sure if that includes the http headers so 175 sounds fine...
#3) Try uncommenting the WStackedWidget's animation code, because I think it needed a css file that (1)'s resource hack thing brings in
#4) Uncomment DEPLOY VARIABLES below [and comment-out or delete TESTING VARIABLES]
#5) Turn on auto-failover
#5) cd into wtAppHere and `ln -s /usr/local/share/Wt/resources/ resources`
#                                   ^or just /usr/share/Wt if using Wt from debian repository
#6) Launch with ./wtAppHere --docroot ".;/resources" --http-address 0.0.0.0 --http-port 7777

TARGET = AnonymousBitcoinComputing
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -save-temps

#DEFINES += ABC_MULTI_CAMPAIGN_OWNER_MODE

HEADERS += \
    anonymousbitcoincomputing.h \
    frontend/anonymousbitcoincomputingwtgui.h \
    backend/anonymousbitcoincomputingcouchbasedb.h \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.h \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.h \
    backend/getandsubscribecacheitem.h \
    frontend/accounttabs/filedeletingfileresource.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    frontend/registersuccessfulwidget.h \
    frontend/validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h \
    frontend/validatorsandinputfilters/safetextvalidatorandinputfilter.h \
    frontend/accounttabs/addfundsaccounttabbody.h \
    frontend/accounttabs/newadslotfilleraccounttabbody.h \
    frontend/accounttabs/viewallexistingadslotfillersaccounttabbody.h \
    frontend/accounttabs/iaccounttabwidgettabbody.h \
    frontend/accounttabs/actuallazyloadedtabwidget.h \
    frontend/accounttabs/singleuseselfdeletingmemoryresource.h \
    frontend/accounttabs/stupidmimefromextensionutil.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/iautoretryingwithexponentialbackoffcouchbaserequest.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasegetrequest.h \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.h \
    ../../../GloballySharedClasses/distributeddatabase/d3faultscouchbaseshared.h \
    ../../../GloballySharedClasses/image/nonanimatedimagheaderchecker.h \
    abc2common.h \
    ../../hackyVideoBullshitSite/src/backend/nonexpiringstringwresource.h

SOURCES += main.cpp \
    anonymousbitcoincomputing.cpp \
    frontend/anonymousbitcoincomputingwtgui.cpp \
    backend/anonymousbitcoincomputingcouchbasedb.cpp \
    frontend2backendRequests/getcouchbasedocumentbykeyrequest.cpp \
    frontend2backendRequests/storecouchbasedocumentbykeyrequest.cpp \
    frontend/accounttabs/filedeletingfileresource.cpp \
    frontend/registersuccessfulwidget.cpp \
    frontend/validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.cpp \
    frontend/validatorsandinputfilters/safetextvalidatorandinputfilter.cpp \
    frontend/accounttabs/addfundsaccounttabbody.cpp \
    frontend/accounttabs/newadslotfilleraccounttabbody.cpp \
    frontend/accounttabs/viewallexistingadslotfillersaccounttabbody.cpp \
    frontend/accounttabs/actuallazyloadedtabwidget.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    frontend/accounttabs/singleuseselfdeletingmemoryresource.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/iautoretryingwithexponentialbackoffcouchbaserequest.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasegetrequest.cpp \
    backend/autoretryingwithexponentialbackoffcouchbaserequests/autoretryingwithexponentialbackoffcouchbasestorerequest.cpp \
    ../../hackyVideoBullshitSite/src/backend/nonexpiringstringwresource.cpp

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/
INCLUDEPATH += ../../../GloballySharedClasses/image/
INCLUDEPATH += ../../hackyVideoBullshitSite/src/backend/

LIBS += -lcouchbase -levent -levent_pthreads -lwt -lwthttp -lboost_signals -lboost_system -lboost_thread -lboost_serialization #TODOoptional: serialization lib not necessary when using lockfree queue
