#-------------------------------------------------
#
# Project created by QtCreator 2013-01-16T09:12:52
#
#-------------------------------------------------

QT       += core dbus xml network

QT       -= gui

TARGET = ssolib
QMAKE_CXXFLAGS += -std=c++0x
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = lib


SOURCES += \
    keyring/*.cpp \
    dbus/*.cpp \
    sso_api/*.cpp \
    main/*.cpp \
    ssoservice.cpp

HEADERS += \
    keyring/*.h \
    dbus/*.h \
    sso_api/*.h \
    main/*.h \
    ssoservice.h \
    main/user.h

