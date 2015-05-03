#ifndef FACEBOOKRSSIMAGEDOWNLOADER_H
#define FACEBOOKRSSIMAGEDOWNLOADER_H

#include <QObject>

class FacebookRssReaderItem;
class FacebookRssImageDownloaderPrivate;
class FacebookRssImageDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString destination READ destination WRITE setDestination NOTIFY destinationChanged)

public:
    FacebookRssImageDownloader(QObject *parent = 0);
    ~FacebookRssImageDownloader();

    void setDestination(const QString &dest);
    QString destination() const;

public slots:
    void downloadImages(FacebookRssReaderItem *item);

signals:
    void destinationChanged();

signals:
    void finished(const QList<FacebookRssReaderItem*> &items);

private slots:
    void finished_prv(qint64 id, const QString &path);

private:
    void checkFinished();

private:
    FacebookRssImageDownloaderPrivate *p;
};

#endif // FACEBOOKRSSIMAGEDOWNLOADER_H
