#include "jamiatserverdaemon.h"

#include <QProcess>
#include <QCoreApplication>
#include <QPointer>
#include <QTimer>
#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QTimer>

class JamiatServerDaemonPrivate
{
public:
    QPointer<QProcess> apiProccess;
    QPointer<QProcess> downloaderProccess;

    QString logPath;

    QPointer<QFile> apiLog;
    QPointer<QFile> downloaderLog;

    QTimer *restarter;
};

JamiatServerDaemon::JamiatServerDaemon(QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerDaemonPrivate;
    p->restarter = 0;

    stopAllServices();

    QString config_file = QCoreApplication::applicationDirPath() + "/config.ini";
    if(!QFileInfo::exists(config_file))
        config_file = "/etc/sosa/config.ini";

    Q_ASSERT_X(QFileInfo::exists(config_file), __PRETTY_FUNCTION__, "Cannot open config file.");

    QSettings settings(config_file, QSettings::IniFormat);

    p->logPath = settings.value("log/path", QCoreApplication::applicationDirPath() + "/log").toString();
    QDir().mkpath(p->logPath);
}

void JamiatServerDaemon::start()
{
    startApiService();
    startDownloaderService();

    if(!p->restarter)
    {
        p->restarter = new QTimer(this);
        p->restarter->setSingleShot(false);
        p->restarter->setInterval(12*60*60*1000);
        p->restarter->start();

        connect(p->restarter, SIGNAL(timeout()), SLOT(restart()));
    }
}

void JamiatServerDaemon::stop()
{
    stopApiService();
    stopDownloaderService();

    if(p->restarter)
        delete p->restarter;
    p->restarter = 0;
}

void JamiatServerDaemon::stopAll()
{
    stopAllServices();
    QProcess::execute("killall", QStringList()<<"JamiatServerDaemon");
}

void JamiatServerDaemon::stopAllServices()
{
    QProcess::execute("killall", QStringList()<<"JamiatServerApi");
    QProcess::execute("killall", QStringList()<<"JamiatServerDownloader");
}

void JamiatServerDaemon::restart()
{
    stopAllServices();
}

void JamiatServerDaemon::startApiService()
{
    if(p->apiProccess)
        return;

    p->apiProccess = new QProcess(this);
#ifdef QT_DEBUG
    p->apiProccess->setProcessChannelMode(QProcess::ForwardedChannels);
#else
    p->apiProccess->setProcessChannelMode(QProcess::MergedChannels);

    connect(p->apiProccess, SIGNAL(readyReadStandardError()), SLOT(apiReadyRead()));
    connect(p->apiProccess, SIGNAL(readyReadStandardOutput()), SLOT(apiReadyRead()));
#endif

    connect(p->apiProccess, SIGNAL(finished(int)), SLOT(apiFinished(int)));

    p->apiProccess->start(QCoreApplication::applicationDirPath() + "/JamiatServerApi");
}

void JamiatServerDaemon::stopApiService()
{
    p->apiProccess->terminate();
    p->apiProccess->deleteLater();
    p->apiProccess = 0;
}

void JamiatServerDaemon::startDownloaderService()
{
    if(p->downloaderProccess)
        return;

    p->downloaderProccess = new QProcess(this);
#ifdef QT_DEBUG
    p->downloaderProccess->setProcessChannelMode(QProcess::ForwardedChannels);
#else
    p->downloaderProccess->setProcessChannelMode(QProcess::MergedChannels);

    connect(p->downloaderProccess, SIGNAL(readyReadStandardError()), SLOT(downloaderReadyRead()));
    connect(p->downloaderProccess, SIGNAL(readyReadStandardOutput()), SLOT(downloaderReadyRead()));
#endif

    connect(p->downloaderProccess, SIGNAL(finished(int)), SLOT(downloaderFinished(int)));

    p->downloaderProccess->start(QCoreApplication::applicationDirPath() + "/JamiatServerDownloader");
}

void JamiatServerDaemon::stopDownloaderService()
{
    if(!p->downloaderProccess)
        return;

    p->downloaderProccess->terminate();
    p->downloaderProccess->deleteLater();
    p->downloaderProccess = 0;
}

void JamiatServerDaemon::apiFinished(int res)
{
    Q_UNUSED(res)
    if(p->apiProccess)
    {
        stopApiService();
        QTimer::singleShot(5000, this, SLOT(startApiService()));
    }
}

void JamiatServerDaemon::downloaderFinished(int res)
{
    Q_UNUSED(res)
    if(p->downloaderProccess)
    {
        stopDownloaderService();
        QTimer::singleShot(5000, this, SLOT(startDownloaderService()));
    }
}

void JamiatServerDaemon::apiReadyRead()
{
    if(!p->apiLog)
    {
        p->apiLog = new QFile(p->logPath + "/JamiatServerApi", this);
        p->apiLog->open(QFile::WriteOnly);
    }

    p->apiLog->write(p->apiProccess->readAll());
    p->apiLog->flush();
}

void JamiatServerDaemon::downloaderReadyRead()
{
    if(!p->downloaderLog)
    {
        p->downloaderLog = new QFile(p->logPath + "/JamiatServerDownloader", this);
        p->downloaderLog->open(QFile::WriteOnly);
    }

    p->downloaderLog->write(p->downloaderProccess->readAll());
    p->downloaderLog->flush();
}

JamiatServerDaemon::~JamiatServerDaemon()
{
    stop();
    delete p;
}

