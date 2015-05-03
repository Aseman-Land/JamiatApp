#include <QCoreApplication>
#include <QNetworkProxy>

#include "jamiatserverdownloadercore.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JamiatServerDownloaderCore server;
    server.start();

    return a.exec();
}
