#-------------------------------------------------
#
# Project created by QtCreator 2010-12-05T10:51:11
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = motiondetector
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS +=-lcxcore -lcv -lhighgui -lcvaux -lml

SOURCES += main.cpp \
    motiondetector.cpp \
    patchItem.cpp

HEADERS += \
    patchItem.h \
    motiondetector.h
