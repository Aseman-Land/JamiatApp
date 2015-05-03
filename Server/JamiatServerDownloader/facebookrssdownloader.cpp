#include "facebookrssdownloader.h"
#include "asemandownloader.h"
#include "facebookrssimagedownloader.h"
#include "jamiatdatabase.h"

#include <QPointer>
#include <QUrl>
#include <QFile>
#include <QStringList>
#include <QDomDocument>
#include <QVariant>
#include <QHash>
#include <QDebug>
#include <QProcess>

QHash<QString,FacebookRssReaderItem*> rss_downloader_hash;
QList<FacebookRssReaderItem*> rss_downloader_list;

class RssDownloaderPrivate
{
public:
    QPointer<AsemanDownloader> downloader;
    QPointer<FacebookRssImageDownloader> imageDownloader;
    JamiatDatabase *database;
    QString accountId;

};

FacebookRssDownloader::FacebookRssDownloader(JamiatDatabase *database, QObject *parent) :
    QObject(parent)
{
    p = new RssDownloaderPrivate;
    p->imageDownloader = new FacebookRssImageDownloader(this);
    p->database = database;

    connect(p->imageDownloader, SIGNAL(finished(QList<FacebookRssReaderItem*>)), SLOT(img_finished(QList<FacebookRssReaderItem*>)));
    connect(p->imageDownloader, SIGNAL(destinationChanged())                   , SIGNAL(imagesDestinationChanged())               );
}

void FacebookRssDownloader::setAccountId(const QString &id)
{
    if(p->accountId == id)
        return;

    p->accountId = id;
    emit accountIdChanged();
}

QString FacebookRssDownloader::accountId() const
{
    return p->accountId;
}

void FacebookRssDownloader::setImagesDestination(const QString &dst)
{
    p->imageDownloader->setDestination(dst);
}

QString FacebookRssDownloader::imagesDestination() const
{
    return p->imageDownloader->destination();
}

QList<FacebookRssReaderItem *> FacebookRssDownloader::list() const
{
    return rss_downloader_list;
}

void FacebookRssDownloader::start()
{
    if(p->downloader)
        return;

    p->downloader = new AsemanDownloader(this);
    p->downloader->setPath(QString("https://www.facebook.com/feeds/page.php?format=rss20&id=%1").arg(p->accountId));

    connect(p->downloader, SIGNAL(finished(QByteArray)), SLOT(finished_prv(QByteArray)), Qt::QueuedConnection);
    connect(p->downloader, SIGNAL(error(QStringList))  , SLOT(error_prv(QStringList))  , Qt::QueuedConnection);

    p->downloader->start();
    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << QDateTime::currentDateTime() << "Start checking sosa page.";
}

void FacebookRssDownloader::stop()
{
    delete p->downloader;
}

void FacebookRssDownloader::finished_prv(const QByteArray &data)
{
    destroyDownloader();
    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << QDateTime::currentDateTime() << "Checking finished";

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument dom;

    if(!dom.setContent(data, true, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << QString("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr);
        return;
    }

    QDomElement root = dom.documentElement();
    if(root.tagName() != "rss")
    {
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << QString("The file is not an TS file.");
        return;
    }
    else
    if(root.hasAttribute("version") && root.attribute("version") < "2.0")
    {
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << QString("The file has old version.");
        return;
    }

    QDomElement child = root.firstChildElement("channel");
    if(child.isNull())
        return;

    QList<FacebookRssReaderItem*> result;
    QDomElement item = child.firstChildElement("item");
    while(!item.isNull())
    {
        FacebookRssReaderItem *obj = parseItemElement(item);
        if(obj)
            result << obj;

        item = item.nextSiblingElement("item");
    }
    if(result.isEmpty())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "There is no new item to add to database.";

    /*! Cleaning up */
    QHashIterator<QString,FacebookRssReaderItem*> i(rss_downloader_hash);
    while(i.hasNext())
    {
        i.next();
        if(!result.contains(i.value()))
            rss_downloader_hash.remove(i.key());
    }
    foreach(FacebookRssReaderItem *item, rss_downloader_list)
        if(!result.contains(item))
            rss_downloader_list.removeAll(item);


    emit finished();
}

void FacebookRssDownloader::error_prv(const QStringList &error)
{
    destroyDownloader();
    qDebug() << error.join("\n");
    emit FacebookRssDownloader::error();
}

void FacebookRssDownloader::img_finished(const QList<FacebookRssReaderItem *> &items)
{
    rss_downloader_list << items;
    emit listChanged(items);
}

FacebookRssReaderItem *FacebookRssDownloader::parseItemElement(const QDomElement &element)
{
    QString guid;
    QString title;
    QString link;
    QString html;
    QString description;
    QDateTime pubDate;
    QString author;
    QString creator;
    QString eventId;

    QDomElement child = element.firstChildElement();
    while (!child.isNull())
    {
        const QString &tag = child.tagName();
        const QString &data = convertHtmlToText(child.text());

        if(tag == "guid")
            guid = data.mid(data.lastIndexOf("/")+1);
        else
        if(tag == "link")
            link = child.text();
        else
        if(tag == "description")
        {
            html = child.text();
            description = data;
        }
        else
        if(tag == "pubDate")
            pubDate = QDateTime::fromString(data.mid(0, data.lastIndexOf(" ")), "ddd, dd MMM yyyy HH:mm:ss");
        else
        if(tag == "author")
            author = data;
        else
        if(tag == "creator")
            creator = data;

        child = child.nextSiblingElement();
    }

    if(p->database->contains(guid))
        return 0;
    if(rss_downloader_hash.contains(guid))
        return rss_downloader_hash.value(guid);

    title = description.left(description.indexOf("\n", 10));
    title = title.left(64);
    if(title.size() == 64)
        title += "...";

    FacebookRssReaderItem *result = new FacebookRssReaderItem;
    result->setGuid(guid);
    result->setTitle(title);
    result->setLink(link);
    result->setHtml(html);
    result->setDescription(description);
    result->setPubDate(pubDate);
    result->setAuthor(author);
    result->setCreator(creator);
    result->setEventId( extractEventId(description) );
    result->setType( result->eventId().isEmpty()? FacebookRssReaderItem::NormalPost : FacebookRssReaderItem::EventPost );

    p->imageDownloader->downloadImages(result);

    rss_downloader_hash[guid] = result;
    return result;
}

QString FacebookRssDownloader::convertHtmlToText(const QString &html)
{
    const QString htmlPath = "/tmp/jamiat_html_file.html";
    QFile htmlFile(htmlPath);
    if(!htmlFile.open(QFile::WriteOnly))
        return QString();

    htmlFile.write(html.toUtf8());
    htmlFile.close();

    QProcess process;
    process.start("html2text", QStringList()<<"-utf8"<<htmlPath);
    process.waitForStarted();
    process.waitForReadyRead();
    process.waitForFinished();

    htmlFile.remove();

    return process.readAll().trimmed();
}

void FacebookRssDownloader::destroyDownloader()
{
    if(!p->downloader)
        return;

    p->downloader->deleteLater();
    p->downloader = 0;
}

QString FacebookRssDownloader::extractEventId(const QString &text)
{
    QRegExp rx("facebook\\.com\\/events\\/(\\d+)");
    int pos = 0;

    if((pos = rx.indexIn(text, pos)) == -1)
        return QString();
    else
        return rx.cap(1);
}

FacebookRssDownloader::~FacebookRssDownloader()
{
    delete p;
}

