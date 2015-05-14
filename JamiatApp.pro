TEMPLATE = app
QT += qml quick xml sql

folder_01.source = translations
folder_01.target = $$DESTDIR/files
folder_02.source = fonts
folder_02.target = $$DESTDIR/.
folder_03.source = database
folder_03.target = $$DESTDIR/.
DEPLOYMENTFOLDERS += folder_01 folder_02 folder_03

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

SOURCES += main.cpp \
    mainpagemodel.cpp \
    jamiatapp.cpp \
    rssreader.cpp \
    mainservice.cpp \
    jamiatandroidservice.cpp \
    mainpagemodelitem.cpp \
    cachedatabase.cpp \
    apilayer.cpp

RESOURCES += resource.qrc

include(asemantools/asemantools.pri)
include(qmake/qtcAddDeployment.pri)
qtcAddDeployment()

HEADERS += \
    mainpagemodel.h \
    jamiatapp.h \
    rssreader.h \
    jamiatandroidservice.h \
    mainpagemodelitem.h \
    cachedatabase.h \
    apilayer.h

DISTFILES += \
    android/AndroidManifest.xml \
    database/cache.sqlite
