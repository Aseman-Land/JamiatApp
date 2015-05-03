#include "facebookimagedownloader.h"
#include "facebookrssdownloader.h"
#include "asemandownloader.h"

#include <QPointer>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

class FacebookImageDownloaderPrivate
{
public:
    QString link;
    QString link_id;
    QString destination;

    QPointer<AsemanDownloader> downloader;
    QPointer<AsemanDownloader> image_downloader;
};

FacebookImageDownloader::FacebookImageDownloader(QObject *parent) :
    QObject(parent)
{
    p = new FacebookImageDownloaderPrivate;
}

void FacebookImageDownloader::setLink(const QString &link)
{
    if(p->link == link)
        return;
    if(p->downloader || p->image_downloader)
        return;

    p->link = link;
    emit linkChanged();
}

QString FacebookImageDownloader::link() const
{
    return p->link;
}

QString FacebookImageDownloader::fileName() const
{
    return p->link_id;
}

void FacebookImageDownloader::setDestination(const QString &dest)
{
    if(p->destination == dest)
        return;

    p->destination = dest;
    emit destinationChanged();
}

QString FacebookImageDownloader::destination() const
{
    return p->destination;
}

void FacebookImageDownloader::start()
{
    if(p->downloader || p->image_downloader)
        return;

    qDebug() << "Start Downloading" << p->link;
    p->downloader = new AsemanDownloader(this);
    p->downloader->setPath(p->link);

    connect(p->downloader, SIGNAL(finished(QByteArray)), SLOT(finished_prv(QByteArray)), Qt::QueuedConnection);
    connect(p->downloader, SIGNAL(error(QStringList))  , SLOT(error_prv(QStringList))  , Qt::QueuedConnection);

    p->downloader->start();
}

void FacebookImageDownloader::stop()
{
    if(!p->downloader)
        return;

    delete p->downloader;
    p->downloader = 0;
}

void FacebookImageDownloader::finished_prv(const QByteArray &data)
{
    const int idx = data.indexOf("id=\"fbPhotoImage\"");
    const QString str = data.mid(idx, 400);

    QString imageLink;
    QRegExp rx("src\\=\\\"(.+)\\\"");
    rx.setMinimal(true);
    if(rx.indexIn(str) != -1)
    {
        imageLink = FacebookRssDownloader::convertHtmlToText(rx.cap(1)).replace("&amp;", "&").remove("\n");
    }
    else
    {
        destroyDownloader();
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "Could not find image.";
        emit error();
        return;
    }

    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "Image link found:" << imageLink;

    QRegExp rx_link("\\/(\\w+\\.jpg)");
    if(rx_link.indexIn(imageLink) != -1)
        p->link_id = rx_link.cap(1);
    else
        p->link_id.clear();

    emit fileNameChanged();
    destroyDownloader();

    const QString &path = p->destination + "/" + p->link_id;
    if(QFileInfo::exists(path))
    {
        qDebug() << "Image downloaded before:" << p->link_id;
        emit finished(path);
    }
    else
        downloadImage(imageLink);
}

void FacebookImageDownloader::downloadImage(const QString &link)
{
    if(p->image_downloader)
        return;

    p->image_downloader = new AsemanDownloader(this);
    p->image_downloader->setPath(link);

    connect(p->image_downloader, SIGNAL(finished(QByteArray)), SLOT(imageFinished_prv(QByteArray)), Qt::QueuedConnection);
    connect(p->image_downloader, SIGNAL(error(QStringList))  , SLOT(imageError_prv(QStringList))  , Qt::QueuedConnection);

    p->image_downloader->start();
}

void FacebookImageDownloader::error_prv(const QStringList &error)
{
    destroyDownloader();
    qDebug() << error.join("\n");
    emit FacebookImageDownloader::error();
}

void FacebookImageDownloader::imageFinished_prv(const QByteArray &data)
{
    destroyImageDownloader();

    const QString &path = p->destination + "/" + p->link_id;
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "cant open file" << path;
        return;
    }

    file.write(data);
    file.flush();
    file.close();

    emit finished(path);
}

void FacebookImageDownloader::imageError_prv(const QStringList &error)
{
    destroyImageDownloader();
    qDebug() << error.join("\n");
    emit FacebookImageDownloader::error();
}

void FacebookImageDownloader::destroyDownloader()
{
    if(!p->downloader)
        return;

    p->downloader->deleteLater();
    p->downloader = 0;
}

void FacebookImageDownloader::destroyImageDownloader()
{
    if(!p->image_downloader)
        return;

    p->image_downloader->deleteLater();
    p->image_downloader = 0;
}

FacebookImageDownloader::~FacebookImageDownloader()
{
    delete p;
}
