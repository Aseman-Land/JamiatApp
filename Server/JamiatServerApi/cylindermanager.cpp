#include "cylindermanager.h"
#include "cylinderitem.h"

#include <QThread>
#include <QSet>
#include <QDebug>
#include <QDateTime>

class CylinderManagerPrivate
{
public:
    int maximumCylinder;

    QList<CylinderItem*> cylinders;
};

CylinderManager::CylinderManager(QObject *parent) :
    QObject(parent)
{
    p = new CylinderManagerPrivate;
    p->maximumCylinder = 2;

    qRegisterMetaType<qintptr>("qintptr");
}

void CylinderManager::setMaximumCylinder(int max)
{
    if(p->maximumCylinder == max)
        return;

    p->maximumCylinder = max;
    emit maximumCylinderChanged();
}

int CylinderManager::maximumCylinder() const
{
    return p->maximumCylinder;
}

void CylinderManager::handleSocket(qintptr handle)
{
    CylinderItem *item = allocateSocket();
    if(!item)
        return;

    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << p->cylinders.count() << item << handle << item->count()+1;
    QMetaObject::invokeMethod(item, "handleSocket", Q_ARG(qintptr, handle));
}

void CylinderManager::cleanUp()
{
    foreach(CylinderItem *item, p->cylinders)
    {
        QThread *thread = item->thread();
        thread->quit();
        thread->wait();
        thread->deleteLater();
        item->deleteLater();
    }

    p->cylinders.clear();
}

CylinderItem *CylinderManager::minimum(int count) const
{
    QSet<CylinderItem*> set;
    CylinderItem *minimum = 0;
    if(count > p->cylinders.count())
        count = p->cylinders.count();

    while(set.count() < count)
    {
        minimum = 0;
        foreach(CylinderItem *item, p->cylinders)
        {
            if(set.contains(item))
                continue;
            if(minimum && item->count()>=minimum->count())
                continue;

            minimum = item;
        }

        set.insert(minimum);
    }

    return minimum;
}

CylinderItem *CylinderManager::maximum(int count) const
{
    QSet<CylinderItem*> set;
    CylinderItem *maximum = 0;
    if(count > p->cylinders.count())
        count = p->cylinders.count();

    while(set.count() < count)
    {
        maximum = 0;
        foreach(CylinderItem *item, p->cylinders)
        {
            if(set.contains(item))
                continue;
            if(maximum && item->count()<=maximum->count())
                continue;

            maximum = item;
        }

        set.insert(maximum);
    }

    return maximum;
}

CylinderItem *CylinderManager::allocateSocket()
{
    CylinderItem *item = p->maximumCylinder>=p->cylinders.count()? minimum() : maximum(p->maximumCylinder);
    if(item && (item->count() == 0 || p->cylinders.count() >= p->maximumCylinder) )
        return item;

    QThread *thread = new QThread(this);

    item = new CylinderItem();
    item->moveToThread(thread);
    p->cylinders.prepend(item);

    thread->start();
    return item;
}

CylinderManager::~CylinderManager()
{
    cleanUp();
    delete p;
}

