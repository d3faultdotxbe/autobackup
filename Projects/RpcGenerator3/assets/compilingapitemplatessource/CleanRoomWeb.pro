######################################################################
# Automatically generated by qmake (3.0) Thu May 21 00:25:30 2015
######################################################################

QT += core

TARGET = CleanRoomWeb
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

DEFINES += NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE=10

HEADERS += web/cleanroomwebwidget.h \
    web/cleanroomweb.h

SOURCES += web/main.cpp \
    web/cleanroomwebwidget.cpp \
    web/cleanroomweb.cpp

include(cleanroom.pri)

LIBS += -lwt -lwthttp
