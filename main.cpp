#include "asemantools/asemanapplication.h"
#include "jamiatapp.h"

#include <QIcon>

int main(int argc, char *argv[])
{
    AsemanApplication app(argc, argv);
    app.setApplicationName("jamiat");
    app.setApplicationDisplayName("Jamiat");
    app.setOrganizationDomain("NileGroup");
    app.setOrganizationName("Jamiat");
    app.setWindowIcon(QIcon(":/qml/JamiatApp/files/jamiat.png"));

    JamiatApp jamiat;
    jamiat.start();

    return app.exec();
}
