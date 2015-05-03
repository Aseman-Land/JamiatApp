#include "jamiatserver.h"
#include "jamiattcpserver.h"
#include "cylindermanager.h"

#include <QTcpServer>
#include <QStringList>
#include <QPointer>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

class JamiatServerPrivate
{
public:
    QPointer<JamiatTcpServer> server;
    QPointer<CylinderManager> manager;
};

JamiatServer::JamiatServer(QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerPrivate;
}

void JamiatServer::start()
{
    if(p->server)
        return;

    p->manager = new CylinderManager(this);

    p->server = new JamiatTcpServer(this);
    p->server->listen(QHostAddress::Any, 34946);
    p->server->setMaxPendingConnections(10000);

    connect(p->server.data(), SIGNAL(newConnection(qintptr)), SLOT(newConnection(qintptr)));
}

void JamiatServer::stop()
{
    if(!p->server)
        return;

    delete p->server;
    delete p->manager;
}

void JamiatServer::newConnection(qintptr handle)
{
    p->manager->handleSocket(handle);
}

JamiatServer::~JamiatServer()
{
    delete p;
}

