#-------------------------------------------------
#
# Project created by QtCreator 2015-02-26T20:44:05
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = RecursivelyVerifyCustomDetachedGpgSignaturesCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += cli/main.cpp \
    cli/recursivelyverifycustomdetachedgpgsignaturescli.cpp

HEADERS += \
    cli/recursivelyverifycustomdetachedgpgsignaturescli.h

include(recursivelyverifycustomdetachedgpgsignatures.pri)
include(../../shared/recursivecustomdetachedgpgsignatures.pri)
