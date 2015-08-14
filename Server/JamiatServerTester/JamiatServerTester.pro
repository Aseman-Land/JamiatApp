QT += core network
QT -= gui

TARGET = JamiatServerDaemon
TEMPLATE = app
DESTDIR = ../build

SOURCES += \
    main.cpp \
    apilayer.cpp \
    jamiatservertester.cpp \
    jamiatservertesteritem.cpp

HEADERS += \
    apilayer.h \
    jamiatservertester.h \
    jamiatservertesteritem.h
