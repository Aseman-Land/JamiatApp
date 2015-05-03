QT += core network
QT -= gui

TARGET = JamiatServerDaemon
TEMPLATE = app
DESTDIR = ../build

SOURCES += \
    main.cpp \
    jamiatserverdaemon.cpp

HEADERS += \
    jamiatserverdaemon.h
