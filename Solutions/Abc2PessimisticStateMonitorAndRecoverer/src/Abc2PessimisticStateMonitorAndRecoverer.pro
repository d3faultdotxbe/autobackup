#debugging this app on Debian Wheezy x86_64 requires: LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0

TARGET = Abc2PessimisticStateMonitorAndRecoverer
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    abc2pessimisticstatemonitorandrecoverer.cpp \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.cpp \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.cpp

HEADERS += \
    abc2pessimisticstatemonitorandrecoverer.h \
    ../../Abc2couchbaseKeyAndJsonDefines/abc2couchbaseandjsonkeydefines.h \
    ../../../GloballySharedClasses/distributeddatabase/isynchronouslibcouchbaseuser.h

INCLUDEPATH += ../../Abc2couchbaseKeyAndJsonDefines/
INCLUDEPATH += ../../../GloballySharedClasses/distributeddatabase/

LIBS += -lcouchbase -lboost_random -lboost_filesystem
