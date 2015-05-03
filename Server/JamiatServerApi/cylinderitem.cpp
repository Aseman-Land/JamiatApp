#include "cylinderitem.h"
#include "jamiatserversocket.h"

#include <QTcpSocket>
#include <QMutex>
#include <QUuid>
#include <QDebug>
#include <QSqlDatabase>

class CylinderItemPrivate
{
public:
    QHash<qintptr, JamiatServerSocket*> sockets;
    QMutex mutex;
    QString databaseConnection;
};

CylinderItem::CylinderItem(QObject *parent) :
    QObject(parent)
{
    p = new CylinderItemPrivate;
    p->databaseConnection = QUuid::createUuid().toString();
}

int CylinderItem::count() const
{
    p->mutex.lock();
    int res = p->sockets.count();
    p->mutex.unlock();

    return res;
}

void CylinderItem::handleSocket(qintptr handle)
{
    JamiatServerSocket *socket = new JamiatServerSocket(handle, p->databaseConnection, this);
    p->mutex.lock();
    p->sockets[handle] = socket;
    p->mutex.unlock();

    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
    socket->start();
}

void CylinderItem::disconnected()
{
    JamiatServerSocket *socket = static_cast<JamiatServerSocket*>(sender());
    if(!socket)
        return;

    p->mutex.lock();
    p->sockets.remove(socket->socketDescriptor());
    p->mutex.unlock();

    qDebug() << socket << socket->socketDescriptor() << "Disconnected";
    delete socket;
}

CylinderItem::~CylinderItem()
{
    if(QSqlDatabase::contains(p->databaseConnection))
        QSqlDatabase::removeDatabase(p->databaseConnection);

    delete p;
}

