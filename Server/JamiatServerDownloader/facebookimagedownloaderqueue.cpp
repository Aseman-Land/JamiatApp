#include "facebookimagedownloaderqueue.h"
#include "facebookimagedownloader.h"

#include <QQueue>
#include <QStack>
#include <QHash>
#include <QDir>
#include <QDebug>

class FacebookImageDownloaderQueuePrivate
{
public:
    int queueCount;
    QString destination;

    QStack<FacebookImageDownloader*> deactiveDownloaders;
    QList<FacebookImageDownloader*> activeDownloaders;

    QQueue<QString> queue;
    QHash<QString, qint64> ids;
    qint64 idsPointer;
};

FacebookImageDownloaderQueue::FacebookImageDownloaderQueue(QObject *parent) :
    QObject(parent)
{
    p = new FacebookImageDownloaderQueuePrivate;
    p->queueCount = 2;
    p->idsPointer = 0;
}

void FacebookImageDownloaderQueue::setQueueCount(int count)
{
    if(p->queueCount == count)
        return;

    p->queueCount = count;
    emit queueCountChanged();
}

int FacebookImageDownloaderQueue::queueCount() const
{
    return p->queueCount;
}

void FacebookImageDownloaderQueue::setDestination(const QString &dest)
{
    if(p->destination == dest)
        return;

    QDir().mkpath(dest);
    p->destination = dest;

    foreach(FacebookImageDownloader *item, p->deactiveDownloaders)
        item->setDestination(dest);
    foreach(FacebookImageDownloader *item, p->activeDownloaders)
        item->setDestination(dest);

    emit destinationChanged();
}

QString FacebookImageDownloaderQueue::destination() const
{
    return p->destination;
}

qint64 FacebookImageDownloaderQueue::download(const QString &link)
{
    p->idsPointer++;
    p->ids[link] = p->idsPointer;
    p->queue.append(link);

    next();

    return p->idsPointer;
}

void FacebookImageDownloaderQueue::finished_prv(const QString &path)
{
    FacebookImageDownloader *downloader = static_cast<FacebookImageDownloader*>(sender());
    if(!downloader)
        return;

    p->activeDownloaders.removeOne(downloader);
    p->deactiveDownloaders.push(downloader);

    const QString &link = downloader->link();
    const qint64 id = p->ids.take(link);

    next();

    emit finished(id, path);
}

void FacebookImageDownloaderQueue::error_prv()
{
    finished_prv(QString());
}

void FacebookImageDownloaderQueue::next()
{
    if(p->queue.isEmpty())
        return;

    FacebookImageDownloader *downloader = getDownloader();
    if(!downloader)
        return;

    downloader->setDestination(p->destination);
    downloader->setLink(p->queue.takeFirst());
    downloader->start();
}

FacebookImageDownloader *FacebookImageDownloaderQueue::getDownloader()
{
    if(p->activeDownloaders.count() >= p->queueCount)
        return 0;

    FacebookImageDownloader *result = p->deactiveDownloaders.isEmpty()? 0 : p->deactiveDownloaders.pop();
    if(!result)
    {
        result = new FacebookImageDownloader(this);
        p->activeDownloaders.append(result);

        connect(result, SIGNAL(finished(QString)), SLOT(finished_prv(QString)));
        connect(result, SIGNAL(error())          , SLOT(error_prv())          );
    }

    return result;
}

FacebookImageDownloaderQueue::~FacebookImageDownloaderQueue()
{
    delete p;
}

