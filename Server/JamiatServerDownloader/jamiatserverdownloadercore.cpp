#include "jamiatserverdownloadercore.h"
#include "facebookrssdownloader.h"
#include "jamiatdatabase.cpp"

#include <QTimer>
#include <QPointer>
#include <QFile>
#include <QUuid>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

class JamiatServerDownloaderCorePrivate
{
public:
    QPointer<FacebookRssDownloader> downloader;
    JamiatDatabase *database;
    QTimer *timer;
};

JamiatServerDownloaderCore::JamiatServerDownloaderCore(QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerDownloaderCorePrivate;

    p->database = new JamiatDatabase(this);

    p->timer = new QTimer(this);
    p->timer->setSingleShot(false);
    p->timer->setInterval(30*60*1000);

    connect(p->timer, SIGNAL(timeout()), SLOT(initAndStart()));
}

void JamiatServerDownloaderCore::initAndStart()
{
//    if(p->downloader)
//        p->downloader->deleteLater();

    QString config_file = QCoreApplication::applicationDirPath() + "/config.ini";
    if(!QFileInfo::exists(config_file))
        config_file = "/etc/sosa/config.ini";

    Q_ASSERT_X(QFileInfo::exists(config_file), __PRETTY_FUNCTION__, "Cannot open config file.");

    QSettings settings(config_file, QSettings::IniFormat);

    p->downloader = new FacebookRssDownloader(p->database, this);
    p->downloader->setAccountId("115484678470735");
    p->downloader->setImagesDestination(settings.value("images/path").toString());

    Q_ASSERT_X(!p->downloader->imagesDestination().isEmpty(), __PRETTY_FUNCTION__, "image download path is not set.");

    connect(p->downloader, SIGNAL(listChanged(QList<FacebookRssReaderItem*>)), SLOT(listChanged(QList<FacebookRssReaderItem*>)));

    p->downloader->start();
}

void JamiatServerDownloaderCore::start()
{
    p->timer->stop();
    p->timer->start();
    initAndStart();
}

void JamiatServerDownloaderCore::stop()
{
    if(p->downloader)
        p->downloader->deleteLater();

    p->timer->stop();
}

void JamiatServerDownloaderCore::listChanged(const QList<FacebookRssReaderItem *> &list)
{
    foreach(FacebookRssReaderItem *item, list)
    {
        p->database->insert(item);
        qDebug() << "Item writted to database:" << item->guid();
    }
}

JamiatServerDownloaderCore::~JamiatServerDownloaderCore()
{
    delete p;
}

