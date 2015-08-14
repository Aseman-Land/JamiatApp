#include "jamiatservertester.h"

#include <QCoreApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JamiatServerTester tester;
    tester.start();

    return a.exec();
}
