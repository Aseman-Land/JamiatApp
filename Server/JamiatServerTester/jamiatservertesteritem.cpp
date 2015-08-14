#include "jamiatservertesteritem.h"

#include <QPointer>
#include <QTimer>

class JamiatServerTesterItemPrivate
{
public:
    ApiLayer *api;
    QTimer *timer;
    QSet<qint64> requests;
    int reqCount;
};

JamiatServerTesterItem::JamiatServerTesterItem(QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerTesterItemPrivate;
    p->reqCount = 0;

    p->api = new ApiLayer(this);

    p->timer = new QTimer(this);
    p->timer->setInterval(100);
    p->timer->setSingleShot(false);

    connect(p->timer, SIGNAL(timeout()), SLOT(sendRequest()));
    connect(p->api, SIGNAL(updateRequestAnswer(qint64,QList<ApiLayer_ItemStruct>)),
            SLOT(updateRequestAnswer(qint64,QList<ApiLayer_ItemStruct>)));
}

void JamiatServerTesterItem::start()
{
    if(p->timer->isActive())
        return;

    p->timer->start();
}

void JamiatServerTesterItem::sendRequest()
{
    if(p->reqCount >= 100)
    {
        p->timer->stop();
        qDebug() << this << "Requests done";
        return;
    }

    qint64 rid = p->api->updateRequest();
    p->requests.insert(rid);

    p->reqCount++;
}

void JamiatServerTesterItem::updateRequestAnswer(qint64 id, const QList<ApiLayer_ItemStruct> &items)
{
    qDebug() << this << __PRETTY_FUNCTION__ << items.count();

    p->requests.remove(id);
    if(p->reqCount >= 100 && p->requests.isEmpty())
        qDebug() << this << "Done :)";
}

JamiatServerTesterItem::~JamiatServerTesterItem()
{
    delete p;
}

