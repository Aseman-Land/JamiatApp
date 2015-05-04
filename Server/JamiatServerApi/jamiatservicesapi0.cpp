#include "jamiatservicesapi0.h"

#include <QDataStream>
#include <QDateTime>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

class JamiatServicesApi0_UpdateUnitStruct
{
public:
    QString guid;
    QString link;
    QString title;
    QString description;
    QDateTime date;
    QString publisher;
    QStringList pictures;
    QStringList thumbnails;
    QString eventId;
    int type;
};

class JamiatServicesApi0Private
{
public:
    QSqlDatabase db;
};

JamiatServicesApi0::JamiatServicesApi0(const QString &connection, QObject *parent) :
    QObject(parent)
{
    p = new JamiatServicesApi0Private;
    p->db = QSqlDatabase::database(connection);
}

QByteArray JamiatServicesApi0::updateService(QByteArray data)
{
    QByteArray result;
    QDataStream ostream(&result, QIODevice::WriteOnly);
    QDataStream istream(&data, QIODevice::ReadOnly);

    int structId = 0;
    qint64 dateFrom = 0;
    int offset = 0;
    int limit = 0;
    qint64 attachId = 0;

    istream >> structId;
    istream >> dateFrom;
    istream >> offset;
    istream >> limit;
    istream >> attachId;

    if(structId != UpdateRequestStruct)
        return result;

    if(limit <= 0 || limit > 20)
        limit = 20;
    if(dateFrom <= 0)
        dateFrom = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QSqlQuery query(p->db);
    query.prepare("SELECT guid, link, title, description, date, publisher, GROUP_CONCAT(picture, ','), GROUP_CONCAT(thumbnail, ','), type, eventId "
                  "FROM post "
                  "LEFT JOIN image "
                  "ON post.id = image.post_id "
                  "WHERE post.date < :fromdate AND post.date > :todate "
                  "GROUP BY guid "
                  "ORDER BY post.date DESC "
                  "LIMIT :limit "
                  "OFFSET :offset");

    query.bindValue(":fromdate", QDateTime::fromMSecsSinceEpoch(dateFrom) );
    query.bindValue(":todate", QDateTime(QDate(1,1,1), QTime(0,0)) );
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();

    QList<JamiatServicesApi0_UpdateUnitStruct> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        JamiatServicesApi0_UpdateUnitStruct unit;
        unit.guid = record.value("guid").toString();
        unit.link = record.value("link").toString();
        unit.title = record.value("title").toString();
        unit.description = record.value("description").toString();
        unit.date = record.value("date").toDateTime();
        unit.publisher = record.value("publisher").toString();
        unit.eventId = record.value("eventId").toString();
        unit.pictures = record.value("GROUP_CONCAT(picture, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.thumbnails = record.value("GROUP_CONCAT(thumbnail, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.type = record.value("type").toInt();

        list << unit;
    }

    ostream << static_cast<int>(UpdateStruct);
    ostream << attachId;
    ostream << list.count();
    foreach(const JamiatServicesApi0_UpdateUnitStruct &unit, list)
    {
        ostream << unit.guid;
        ostream << unit.link;
        ostream << unit.title;
        ostream << unit.description;
        ostream << unit.date;
        ostream << unit.publisher;
        ostream << unit.pictures;
        ostream << unit.thumbnails;
        ostream << unit.eventId;
        ostream << unit.type;
    }

    return result;
}

QByteArray JamiatServicesApi0::fullPostService(QByteArray data)
{
    QByteArray result;
    QDataStream ostream(&result, QIODevice::WriteOnly);
    QDataStream istream(&data, QIODevice::ReadOnly);

    int structId = 0;
    QString postGuid;
    qint64 attachId = 0;

    istream >> structId;
    istream >> postGuid;
    istream >> attachId;

    if(structId != FullPostRequestStruct)
        return result;
    if(postGuid.isEmpty())
        return result;

    QSqlQuery query(p->db);
    query.prepare("SELECT guid, link, title, description, date, publisher, GROUP_CONCAT(picture, ','), GROUP_CONCAT(thumbnail, ','), type, eventId "
                  "FROM post "
                  "LEFT JOIN image "
                  "ON post.id = image.post_id "
                  "WHERE post.guid=:guid "
                  "GROUP BY image.post_id "
                  "ORDER BY post.date DESC ");

    query.bindValue(":guid", postGuid );
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();

    JamiatServicesApi0_UpdateUnitStruct unit;
    if(!query.next())
        return result;

    QSqlRecord record = query.record();
    unit.guid = record.value("guid").toString();
    unit.link = record.value("link").toString();
    unit.title = record.value("title").toString();
    unit.description = record.value("description").toString();
    unit.date = record.value("date").toDateTime();
    unit.publisher = record.value("publisher").toString();
    unit.pictures = record.value("GROUP_CONCAT(picture, ',')").toString().split(",", QString::SkipEmptyParts);
    unit.thumbnails = record.value("GROUP_CONCAT(thumbnail, ',')").toString().split(",", QString::SkipEmptyParts);
    unit.type = record.value("type").toInt();
    unit.eventId = record.value("eventId").toString();

    ostream << static_cast<int>(FullPostStruct);
    ostream << attachId;
    ostream << unit.guid;
    ostream << unit.link;
    ostream << unit.title;
    ostream << unit.description;
    ostream << unit.date;
    ostream << unit.publisher;
    ostream << unit.pictures;
    ostream << unit.thumbnails;
    ostream << unit.eventId;
    ostream << unit.type;

    return result;
}

QByteArray JamiatServicesApi0::searchService(QByteArray data)
{
    QByteArray result;
    QDataStream ostream(&result, QIODevice::WriteOnly);
    QDataStream istream(&data, QIODevice::ReadOnly);

    int structId = 0;
    int limit = 0;
    int offset = 0;
    QString keyword;
    qint64 attachId = 0;

    istream >> structId;
    istream >> offset;
    istream >> limit;
    istream >> keyword;
    istream >> attachId;

    if(structId != SearchRequestStruct)
        return result;

    if(limit <= 0 || limit > 20)
        limit = 20;

    QSqlQuery query(p->db);
    query.prepare("SELECT guid, link, title, description, date, publisher, GROUP_CONCAT(picture, ','), GROUP_CONCAT(thumbnail, ','), type, eventId "
                  "FROM post "
                  "LEFT JOIN image "
                  "ON post.id = image.post_id "
                  "WHERE post.description LIKE :keyword "
                  "GROUP BY image.post_id "
                  "ORDER BY post.date DESC "
                  "LIMIT :limit "
                  "OFFSET :offset ");

    query.bindValue(":keyword", "%" + keyword + "%" );
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();

    QList<JamiatServicesApi0_UpdateUnitStruct> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        JamiatServicesApi0_UpdateUnitStruct unit;
        unit.guid = record.value("guid").toString();
        unit.link = record.value("link").toString();
        unit.title = record.value("title").toString();
        unit.description = record.value("description").toString();
        unit.date = record.value("date").toDateTime();
        unit.publisher = record.value("publisher").toString();
        unit.pictures = record.value("GROUP_CONCAT(picture, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.thumbnails = record.value("GROUP_CONCAT(thumbnail, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.type = record.value("type").toInt();
        unit.eventId = record.value("eventId").toString();

        list << unit;
    }

    ostream << static_cast<int>(SearchStruct);
    ostream << attachId;
    ostream << list.count();
    foreach(const JamiatServicesApi0_UpdateUnitStruct &unit, list)
    {
        ostream << unit.guid;
        ostream << unit.link;
        ostream << unit.title;
        ostream << unit.description;
        ostream << unit.date;
        ostream << unit.publisher;
        ostream << unit.pictures;
        ostream << unit.thumbnails;
        ostream << unit.eventId;
        ostream << unit.type;
    }

    return result;
}

QByteArray JamiatServicesApi0::lastEventsService(QByteArray data)
{
    QByteArray result;
    QDataStream ostream(&result, QIODevice::WriteOnly);
    QDataStream istream(&data, QIODevice::ReadOnly);

    int structId = 0;
    QString eventId;
    int limit = 0;
    int offset = 0;
    qint64 attachId = 0;

    istream >> structId;
    istream >> eventId;
    istream >> offset;
    istream >> limit;
    istream >> attachId;

    if(structId != LastEventsRequestStruct)
        return result;

    if(limit <= 0 || limit > 20)
        limit = 20;

    QSqlQuery query(p->db);
    query.prepare(QString(
                  "SELECT guid, link, title, description, date, publisher, GROUP_CONCAT(picture, ','), GROUP_CONCAT(thumbnail, ','), type, eventId "
                  "FROM post "
                  "LEFT JOIN image "
                  "ON post.id = image.post_id "
                  "WHERE post.type=:type ") +
                  QString(eventId.isEmpty()?"" : "AND eventId=:eventId ") + QString(
                  "GROUP BY image.post_id "
                  "ORDER BY post.date DESC "
                  "LIMIT :limit "
                  "OFFSET :offset "));

    query.bindValue(":type", static_cast<int>(EventPost) );
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    query.bindValue(":eventId", eventId);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();

    QList<JamiatServicesApi0_UpdateUnitStruct> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        JamiatServicesApi0_UpdateUnitStruct unit;
        unit.guid = record.value("guid").toString();
        unit.link = record.value("link").toString();
        unit.title = record.value("title").toString();
        unit.description = record.value("description").toString();
        unit.date = record.value("date").toDateTime();
        unit.publisher = record.value("publisher").toString();
        unit.pictures = record.value("GROUP_CONCAT(picture, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.thumbnails = record.value("GROUP_CONCAT(thumbnail, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.type = record.value("type").toInt();
        unit.eventId = record.value("eventId").toString();

        list << unit;
    }

    ostream << static_cast<int>(LastEventsStruct);
    ostream << attachId;
    ostream << list.count();
    foreach(const JamiatServicesApi0_UpdateUnitStruct &unit, list)
    {
        ostream << unit.guid;
        ostream << unit.link;
        ostream << unit.title;
        ostream << unit.description;
        ostream << unit.date;
        ostream << unit.publisher;
        ostream << unit.pictures;
        ostream << unit.thumbnails;
        ostream << unit.eventId;
        ostream << unit.type;
    }

    return result;
}

QByteArray JamiatServicesApi0::fetchEventsService(QByteArray data)
{
    QByteArray result;
    QDataStream ostream(&result, QIODevice::WriteOnly);
    QDataStream istream(&data, QIODevice::ReadOnly);

    int structId = 0;
    qint64 dateFrom = 0;
    int offset = 0;
    int limit = 0;
    qint64 attachId = 0;

    istream >> structId;
    istream >> dateFrom;
    istream >> offset;
    istream >> limit;
    istream >> attachId;

    if(structId != FetchEventsRequestStruct)
        return result;

    if(limit <= 0 || limit > 20)
        limit = 20;
    if(dateFrom <= 0)
        dateFrom = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QSqlQuery query(p->db);
    query.prepare("SELECT * FROM ("
                  "SELECT guid, link, title, description, date, publisher, GROUP_CONCAT(picture, ','), GROUP_CONCAT(thumbnail, ','), type, eventId "
                  "FROM post "
                  "LEFT JOIN image "
                  "ON post.id = image.post_id "
                  "WHERE post.date < :fromdate AND post.date > :todate "
                  "AND post.type=:type "
                  "GROUP BY image.post_id "
                  "ORDER BY post.eventId desc, post.date ASC "
                  ") x GROUP BY eventId "
                  "LIMIT :limit "
                  "OFFSET :offset");

    query.bindValue(":fromdate", QDateTime::fromMSecsSinceEpoch(dateFrom) );
    query.bindValue(":todate", QDateTime(QDate(1,1,1), QTime(0,0)) );
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    query.bindValue(":type", static_cast<int>(EventPost));
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();

    QList<JamiatServicesApi0_UpdateUnitStruct> list;
    while(query.next())
    {
        QSqlRecord record = query.record();
        JamiatServicesApi0_UpdateUnitStruct unit;
        unit.guid = record.value("guid").toString();
        unit.link = record.value("link").toString();
        unit.title = record.value("title").toString();
        unit.description = record.value("description").toString();
        unit.date = record.value("date").toDateTime();
        unit.publisher = record.value("publisher").toString();
        unit.eventId = record.value("eventId").toString();
        unit.pictures = record.value("GROUP_CONCAT(picture, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.thumbnails = record.value("GROUP_CONCAT(thumbnail, ',')").toString().split(",", QString::SkipEmptyParts);
        unit.type = record.value("type").toInt();

        qDebug() << unit.title;

        list << unit;
    }

    ostream << static_cast<int>(FetchEventsStruct);
    ostream << attachId;
    ostream << list.count();
    foreach(const JamiatServicesApi0_UpdateUnitStruct &unit, list)
    {
        ostream << unit.guid;
        ostream << unit.link;
        ostream << unit.title;
        ostream << unit.description;
        ostream << unit.date;
        ostream << unit.publisher;
        ostream << unit.pictures;
        ostream << unit.thumbnails;
        ostream << unit.eventId;
        ostream << unit.type;
    }

    return result;
}

JamiatServicesApi0::~JamiatServicesApi0()
{
    delete p;
}

