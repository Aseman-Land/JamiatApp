#ifndef JAMIATSERVERDAEMON_H
#define JAMIATSERVERDAEMON_H

#include <QObject>

class JamiatServerDaemonPrivate;
class JamiatServerDaemon : public QObject
{
    Q_OBJECT
public:
    JamiatServerDaemon(QObject *parent = 0);
    ~JamiatServerDaemon();

public slots:
    static void stopAll();
    static void stopAllServices();
    static void restart();

    void start();
    void stop();

    void startApiService();
    void stopApiService();

    void startDownloaderService();
    void stopDownloaderService();

private slots:
    void apiFinished(int res);
    void downloaderFinished(int res);

    void apiReadyRead();
    void downloaderReadyRead();

private:
    JamiatServerDaemonPrivate *p;
};

#endif // JAMIATSERVERDAEMON_H
