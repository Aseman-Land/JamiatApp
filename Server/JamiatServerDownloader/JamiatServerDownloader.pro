QT       += core network xml sql
QT -= gui

TARGET = JamiatServerDownloader
TEMPLATE = app
DESTDIR = ../build
DEFINES += QT_FORCE_ASSERTS

SOURCES += main.cpp \
    asemandownloader.cpp \
    facebookrssdownloader.cpp \
    facebookrssreaderitem.cpp \
    jamiatserverdownloadercore.cpp \
    facebookimagedownloader.cpp \
    facebookimagedownloaderqueue.cpp \
    facebookrssimagedownloader.cpp \
    jamiatdatabase.cpp

HEADERS  += \
    asemandownloader.h \
    facebookrssdownloader.h \
    facebookrssreaderitem.h \
    jamiatserverdownloadercore.h \
    facebookimagedownloader.h \
    facebookimagedownloaderqueue.h \
    facebookrssimagedownloader.h \
    jamiatdatabase.h
