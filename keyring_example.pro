#-------------------------------------------------
#
# Project created by QtCreator 2013-01-16T09:12:52
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = keyring_example
CONFIG   += console
CONFIG   -= app_bundle
CONFIG  += qxt
QXT     += core

TEMPLATE = app


SOURCES += main.cpp \
    keyring/*.cpp \
    dbus/*.cpp \
    sso_api/*.cpp \
    core/*.cpp \
    ssoservice.cpp

HEADERS += \
    keyring/*.h \
    dbus/*.h \
    sso_api/*.h \
    core/*.h \
    ssoservice.h \
    core/user.h


QMAKE_CXXFLAGS += -std=c++0x -I /usr/include/qjson
QMAKE_LIBS += -lqjson
