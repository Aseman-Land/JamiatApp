#ifndef JAMIATSERVERCORE_H
#define JAMIATSERVERCORE_H

#include <QObject>

class FacebookRssReaderItem;
class JamiatServerDownloaderCorePrivate;
class JamiatServerDownloaderCore : public QObject
{
    Q_OBJECT
public:
    JamiatServerDownloaderCore(QObject *parent = 0);
    ~JamiatServerDownloaderCore();

public slots:
    void start();
    void stop();

private slots:
    void listChanged(const QList<FacebookRssReaderItem*> &list);
    void initAndStart();

private:
    JamiatServerDownloaderCorePrivate *p;
};

#endif // JAMIATSERVERCORE_H
