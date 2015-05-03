#ifndef FACEBOOKIMAGEDOWNLOADERQUEUE_H
#define FACEBOOKIMAGEDOWNLOADERQUEUE_H

#include <QObject>

class FacebookImageDownloader;
class FacebookImageDownloaderQueuePrivate;
class FacebookImageDownloaderQueue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int queueCount READ queueCount WRITE setQueueCount NOTIFY queueCountChanged)
    Q_PROPERTY(QString destination READ destination WRITE setDestination NOTIFY destinationChanged)

public:
    FacebookImageDownloaderQueue(QObject *parent = 0);
    ~FacebookImageDownloaderQueue();

    void setQueueCount(int count);
    int queueCount() const;

    void setDestination(const QString &dest);
    QString destination() const;

public slots:
    qint64 download(const QString &link);

signals:
    void queueCountChanged();
    void destinationChanged();
    void finished(qint64 id, const QString &path);

private slots:
    void finished_prv(const QString &path);
    void error_prv();

private:
    void next();
    FacebookImageDownloader *getDownloader();

private:
    FacebookImageDownloaderQueuePrivate *p;
};

#endif // FACEBOOKIMAGEDOWNLOADERQUEUE_H
