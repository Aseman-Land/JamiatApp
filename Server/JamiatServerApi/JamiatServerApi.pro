QT += core network sql
QT -= gui

TARGET = JamiatServerApi
TEMPLATE = app
DESTDIR = ../build
DEFINES += QT_FORCE_ASSERTS

SOURCES += \
    main.cpp \
    jamiatserver.cpp \
    jamiattcpserver.cpp \
    cylindermanager.cpp \
    cylinderitem.cpp \
    jamiatserversocket.cpp \
    jamiatservicesapi0.cpp \
    jamiatservicesapi1.cpp

HEADERS += \
    jamiatserver.h \
    jamiattcpserver.h \
    cylindermanager.h \
    cylinderitem.h \
    jamiatserversocket.h \
    jamiatservicesapi0.h \
    jamiatservicesapi1.h
