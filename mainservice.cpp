#include "jamiatandroidservice.h"
#include <QCoreApplication>

extern "C" int mainService(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("jamiat");
    app.setOrganizationDomain("org.aseman.jamiat");
    app.setOrganizationName("Jamiat");

    JamiatAndroidService service;
    service.start();

    return app.exec();
}
