#include "jamiattcpserver.h"

class JamiatTcpServerPrivate
{
public:
};

JamiatTcpServer::JamiatTcpServer(QObject *parent) : QTcpServer(parent)
{
    p = new JamiatTcpServerPrivate;
}

void JamiatTcpServer::incomingConnection(qintptr handle)
{
    emit newConnection(handle);
}

JamiatTcpServer::~JamiatTcpServer()
{
    delete p;
}

