#ifndef FACEBOOKRSSDOWNLOADER_H
#define FACEBOOKRSSDOWNLOADER_H

#include <QObject>
#include "facebookrssreaderitem.h"

class JamiatDatabase;
class QDomElement;
class RssDownloaderPrivate;
class FacebookRssDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString imagesDestination READ imagesDestination WRITE setImagesDestination NOTIFY imagesDestinationChanged)

public:
    FacebookRssDownloader(JamiatDatabase *database, QObject *parent = 0);
    ~FacebookRssDownloader();

    void setAccountId(const QString &id);
    QString accountId() const;

    void setImagesDestination(const QString &dst);
    QString imagesDestination() const;

    QList<FacebookRssReaderItem*> list() const;
    static QString convertHtmlToText(const QString &html);

public slots:
    void start();
    void stop();

signals:
    void finished();
    void error();
    void listChanged(const QList<FacebookRssReaderItem*> &list);
    void accountIdChanged();
    void imagesDestinationChanged();

private slots:
    void finished_prv( const QByteArray & data );
    void error_prv(const QStringList & error);
    void img_finished(const QList<FacebookRssReaderItem*> &items);

private:
    FacebookRssReaderItem *parseItemElement(const QDomElement &element);
    void destroyDownloader();

    QString extractEventId(const QString &text);
    QString extractReportId(const QString &text);

private:
    RssDownloaderPrivate *p;
};

#endif // FACEBOOKRSSDOWNLOADER_H
