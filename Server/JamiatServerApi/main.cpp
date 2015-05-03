#include "jamiatserver.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JamiatServer server;
    server.start();

    return a.exec();
}
