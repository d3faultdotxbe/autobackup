#-------------------------------------------------
#
# Project created by QtCreator 2015-02-22T20:39:41
#
#-------------------------------------------------

QT       += core multimedia serialport
QT       -= gui

TARGET = MusicFingersCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    musicfingerscli.cpp

HEADERS += \
    musicfingerscli.h

include(../../../../GloballySharedClasses/standardinputnotifier.pri)
include(../musicfingers.pri)
include(../musicfingersserialportintegration.pri)
