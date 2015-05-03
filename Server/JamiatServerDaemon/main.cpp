#include "jamiatserverdaemon.h"

#include <QCoreApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(a.arguments().count() == 2 && a.arguments()[1] == "stop")
        JamiatServerDaemon::stopAll();
    else
    if(a.arguments().count() == 2 && a.arguments()[1] == "restart")
        JamiatServerDaemon::restart();
    else
    {
        JamiatServerDaemon daemon;
        daemon.start();

        return a.exec();
    }

    return 0;
}
