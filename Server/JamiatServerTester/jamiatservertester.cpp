#include "jamiatservertester.h"
#include "jamiatservertesteritem.h"

#include <QTimer>
#include <QThread>
#include <QSet>
#include <QDebug>

class JamiatServerTesterPrivate
{
public:
    QTimer *timer;
    QSet<JamiatServerTesterItem*> items;
    int maxThreads;
};

JamiatServerTester::JamiatServerTester(QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerTesterPrivate;
    p->maxThreads = 100;

    p->timer = new QTimer(this);
    p->timer->setInterval(1);
    p->timer->setSingleShot(false);

    connect(p->timer, SIGNAL(timeout()), SLOT(create()));
}

void JamiatServerTester::start()
{
    if(p->timer->isActive())
        return;

    p->timer->start();
}

void JamiatServerTester::create()
{
    if(p->items.count() >= p->maxThreads)
    {
        p->timer->stop();
        return;
    }

    JamiatServerTesterItem *item = new JamiatServerTesterItem();

    QThread *thread = new QThread();
    thread->start();

    item->moveToThread(thread);

    p->items.insert(item);

    QMetaObject::invokeMethod(item, "start", Qt::QueuedConnection);
}

JamiatServerTester::~JamiatServerTester()
{
    delete p;
}

