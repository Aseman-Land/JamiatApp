#include "jamiatserversocket.h"
#include "jamiatservicesapi0.h"

#include <QPointer>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

class JamiatServerSocketPrivate
{
public:
    QPointer<QTcpSocket> socket;
    qintptr descriptor;

    QString databaseConnection;
    JamiatServicesApi0 *api0;
};

JamiatServerSocket::JamiatServerSocket(qintptr handle, const QString &connection, QObject *parent) :
    QObject(parent)
{
    p = new JamiatServerSocketPrivate;
    p->descriptor = handle;
    p->databaseConnection = connection;

    init_db();
    init_apis();
}

void JamiatServerSocket::init_db()
{
    if(QSqlDatabase::contains(p->databaseConnection))
        return;

    QString config_file = QCoreApplication::applicationDirPath() + "/config.ini";
    if(!QFileInfo::exists(config_file))
        config_file = "/etc/sosa/config.ini";

    Q_ASSERT_X(QFileInfo::exists(config_file), __PRETTY_FUNCTION__, "Cannot open config file.");

    QSettings settings(config_file, QSettings::IniFormat);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", p->databaseConnection);
    db.setHostName( settings.value("database/hostname").toString() );
    db.setDatabaseName( settings.value("database/database").toString() );
    db.setUserName( settings.value("database/username").toString() );
    db.setPassword( settings.value("database/password").toString() );

    Q_ASSERT_X(db.open(), __PRETTY_FUNCTION__, "Cannot open database");

    QStringList queries = QStringList() <<
        "CREATE TABLE IF NOT EXISTS post ("
        "id INT(6) UNSIGNED AUTO_INCREMENT,"
        "guid VARCHAR(40) NOT NULL,"
        "link TEXT,"
        "title VARCHAR(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,"
        "description TEXT CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,"
        "date TIMESTAMP,"
        "publisher VARCHAR(30) NOT NULL,"
        "type INT(6) NOT NULL,"
        "eventId VARCHAR(64),"
        "creator VARCHAR(30) NOT NULL,"
        "PRIMARY KEY (id, guid));"
        <<
        "CREATE TABLE IF NOT EXISTS image ("
        "id VARCHAR(128),"
        "post_id INT(6) UNSIGNED,"
        "picture VARCHAR(128) NOT NULL,"
        "thumbnail VARCHAR(128) NOT NULL,"
        "PRIMARY KEY (id),"
        "FOREIGN KEY (post_id)"
        "REFERENCES post(id));"
        <<
        "CREATE INDEX post_guid_idx ON post(guid ASC)"
        <<
        "CREATE INDEX post_date_idx ON post(date ASC)"
        <<
        "CREATE INDEX post_type_idx ON post(type ASC)"
        <<
        "CREATE INDEX post_eventId_idx ON post(eventId ASC)";

    foreach(const QString &query_str, queries)
    {
        QSqlQuery query(db);
        query.prepare(query_str);
        if(!query.exec())
            qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();
    }
}

void JamiatServerSocket::init_apis()
{
    p->api0 = new JamiatServicesApi0(p->databaseConnection, this);
}

void JamiatServerSocket::write(QByteArray data)
{
    p->socket->write(data.replace("\n", "\r") + "\n");
    p->socket->flush();
}

QByteArray JamiatServerSocket::read(qint64 maxlen)
{
    QByteArray data = p->socket->readLine(maxlen);
    data.replace("\n", "");
    data.replace("\r", "\n");

    return data;
}

qintptr JamiatServerSocket::socketDescriptor() const
{
    return p->descriptor;
}

void JamiatServerSocket::start()
{
    if(p->socket)
        return;

    p->socket = new QTcpSocket(this);

    connect(p->socket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(p->socket, SIGNAL(readyRead())   , SLOT(readyRead())     );

    p->socket->setSocketDescriptor(p->descriptor);

    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "new peer socket connected:" << p->socket->peerAddress();
}

void JamiatServerSocket::readyRead()
{
    qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "Got a reply from:" << p->socket->peerAddress();
    const int maxSize = 1<<10;
    while(p->socket->canReadLine())
    {
        QByteArray data = read(maxSize);
        if(data.length() == maxSize)
        {
            p->socket->close();
            emit blockThisShit(p->socket->peerAddress());
            return;
        }

        QDataStream stream(&data, QIODevice::ReadOnly);

        int apiId;
        int serviceId;
        stream >> apiId;
        stream >> serviceId;
        if(stream.atEnd())
            return;

        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << "Got a service request:" << p->socket->peerAddress() << "Service:" << serviceId;
        QByteArray serviceData;
        stream >> serviceData;
        if(serviceData.isEmpty())
            return;

        QByteArray result;
        switch(apiId)
        {
        case JamiatServicesApi0::ApiId:
            result = callServiceApi0(serviceId, serviceData);
            break;
        }

        write(result);
    }
}

QByteArray JamiatServerSocket::callServiceApi0(qint64 id, const QByteArray &data)
{
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream << id;

    switch(id)
    {
    case JamiatServicesApi0::UpdateService:
        stream << p->api0->updateService(data);
        break;

    case JamiatServicesApi0::FullPostService:
        stream << p->api0->fullPostService(data);
        break;

    case JamiatServicesApi0::SearchService:
        stream << p->api0->searchService(data);
        break;

    case JamiatServicesApi0::LastEventsService:
        stream << p->api0->lastEventsService(data);
        break;

    case JamiatServicesApi0::FetchEventsService:
        stream << p->api0->fetchEventsService(data);
        break;

    default:
        qDebug() << id << "there is no any handle for this id";
        break;
    }

    return result;
}

JamiatServerSocket::~JamiatServerSocket()
{
    delete p;
}

