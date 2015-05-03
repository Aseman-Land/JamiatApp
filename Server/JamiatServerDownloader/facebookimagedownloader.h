#ifndef FACEBOOKIMAGEDOWNLOADER_H
#define FACEBOOKIMAGEDOWNLOADER_H

#include <QObject>

class FacebookImageDownloaderPrivate;
class FacebookImageDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString link READ link WRITE setLink NOTIFY linkChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString destination READ destination WRITE setDestination NOTIFY destinationChanged)

public:
    FacebookImageDownloader(QObject *parent = 0);
    ~FacebookImageDownloader();

    void setLink(const QString &link);
    QString link() const;
    QString fileName() const;

    void setDestination(const QString &dest);
    QString destination() const;

public slots:
    void start();
    void stop();

signals:
    void linkChanged();
    void fileNameChanged();
    void destinationChanged();
    void finished(const QString &path);
    void error();

private slots:
    void finished_prv( const QByteArray & data );
    void error_prv(const QStringList & error);

    void imageFinished_prv( const QByteArray & data );
    void imageError_prv(const QStringList & error);

private:
    void destroyDownloader();
    void destroyImageDownloader();
    void downloadImage(const QString &link);

private:
    FacebookImageDownloaderPrivate *p;
};

#endif // FACEBOOKIMAGEDOWNLOADER_H
