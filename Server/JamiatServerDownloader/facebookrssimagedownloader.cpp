#include "facebookrssimagedownloader.h"
#include "facebookimagedownloaderqueue.h"
#include "facebookrssreaderitem.h"

#include <QSet>
#include <QDebug>
#include <QRegExp>
#include <QHash>
#include <QFileInfo>

class FacebookRssImageDownloaderPrivate
{
public:
    FacebookImageDownloaderQueue *queue;

    QSet<FacebookRssReaderItem*> waiting;
    QList<FacebookRssReaderItem*> finished;
    QHash<qint64, FacebookRssReaderItem*> ids;
};

FacebookRssImageDownloader::FacebookRssImageDownloader(QObject *parent) :
    QObject(parent)
{
    p = new FacebookRssImageDownloaderPrivate;
    p->queue = new FacebookImageDownloaderQueue(this);

    connect(p->queue, SIGNAL(destinationChanged())    , SIGNAL(destinationChanged())       );
    connect(p->queue, SIGNAL(finished(qint64,QString)), SLOT(finished_prv(qint64,QString)) );
}

void FacebookRssImageDownloader::setDestination(const QString &dest)
{
    p->queue->setDestination(dest);
}

QString FacebookRssImageDownloader::destination() const
{
    return p->queue->destination();
}

void FacebookRssImageDownloader::downloadImages(FacebookRssReaderItem *item)
{
    if(p->waiting.contains(item))
        return;

    const QString &html = item->html();
    QRegExp rx("\\<a\\s+[^\\>]*href\\=\\\"(.+)\\\"");
    rx.setMinimal(true);

    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(html, pos)) != -1)
    {
        const QString &link = rx.cap(1);
        if(link.contains("/photos/"))
            list << (link.left(1)=="/"? "https://www.facebook.com" + link : link);
        pos += rx.matchedLength();
    }
    if(list.isEmpty())
    {
        p->finished << item;
        checkFinished();
        return;
    }

    item->setLinks(list);
    p->waiting.insert(item);

    foreach(const QString &link, list)
        p->ids[ p->queue->download(link) ] = item;
}

void FacebookRssImageDownloader::finished_prv(qint64 id, const QString &path)
{
    FacebookRssReaderItem *item = p->ids.value(id);
    if(!item)
        return;

    item->setImages( item->images()<<QFileInfo(path).fileName() );
    item->setThumbnails( item->images());

    if(item->images().count() == item->links().count())
    {
        qDebug() << "Image downloads finished" << item << item->guid() << path;
        p->waiting.remove(item);
        p->finished << item;
        qDebug() << p->waiting.count() << " remaining";
    }

    checkFinished();
}

void FacebookRssImageDownloader::checkFinished()
{
    if(!p->waiting.isEmpty())
        return;

    emit finished(p->finished);
    p->finished.clear();
}

FacebookRssImageDownloader::~FacebookRssImageDownloader()
{
    delete p;
}

