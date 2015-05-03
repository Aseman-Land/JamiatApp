#include "cachedatabase.h"
#include "asemantools/asemandevices.h"
#include "asemantools/asemanapplication.h"
#include "apilayer0.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QUuid>
#include <QVariant>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>

class CacheDatabasePrivate
{
public:
    QSqlDatabase db;
    QString path;
};

CacheDatabase::CacheDatabase(QObject *parent) :
    QObject(parent)
{
    p = new CacheDatabasePrivate;
    p->path = AsemanApplication::homePath() + "/cache.sqlite";

    QDir().mkpath(AsemanApplication::homePath());
//    QFile::remove(p->path);
//    QFile::copy(AsemanDevices::resourcePath() + "/database/cache.sqlite", p->path);

    if(!AsemanApplication::settings()->value("initialize/cache-db", false).toBool())
        QFile::copy(AsemanDevices::resourcePath() + "/database/cache.sqlite", p->path);

    AsemanApplication::settings()->setValue("initialize/cache-db", true);
    QFile(p->path).setPermissions(QFileDevice::WriteOwner|QFileDevice::WriteGroup|QFileDevice::ReadUser|QFileDevice::ReadGroup);

    p->db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    p->db.setDatabaseName(p->path);
    p->db.open();
}

void CacheDatabase::write(const QList<ApiLayerItemStruct> &list)
{
    begin();

    foreach(const ApiLayerItemStruct &item, list)
    {
        QSqlQuery query(p->db);
        query.prepare("INSERT OR REPLACE INTO post (guid, link, title, description, date, publisher, pictures, thumbnails, type, eventId) "
                      "VALUES (:guid, :link, :title, :description, :date, :publisher, :pictures, :thumbnails, :type, :eventId);");
        query.bindValue(":guid", item.guid );
        query.bindValue(":link", item.link );
        query.bindValue(":title", item.title );
        query.bindValue(":description", item.description );
        query.bindValue(":date", item.date );
        query.bindValue(":publisher", item.publisher );
        query.bindValue(":pictures", item.pictures.join(", ") );
        query.bindValue(":thumbnails", item.thumbnails.join(", ") );
        query.bindValue(":type", item.type );
        query.bindValue(":eventId", item.eventId );
        if(!query.exec())
            qDebug() << __PRETTY_FUNCTION__ << query.lastError().text();
    }

    commit();
}

QList<ApiLayerItemStruct> CacheDatabase::readUpdates(int offset, int limit) const
{
    QList<ApiLayerItemStruct> result;
    result << readUpdates(0, 1, ApiLayerItemStruct::EventPost);
    result << readUpdates(offset, limit, ApiLayerItemStruct::NormalPost);

    return result;
}

QList<ApiLayerItemStruct> CacheDatabase::readEvents(int limit) const
{
    QList<ApiLayerItemStruct> result;

    QSqlQuery query(p->db);
    query.prepare("SELECT *, MIN(date) FROM post WHERE type=:type "
                  "GROUP BY eventId ORDER BY date DESC LIMIT :limit");
    query.bindValue(":limit", limit);
    query.bindValue(":type", ApiLayerItemStruct::EventPost);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << query.lastError().text();

    while(query.next())
    {
        ApiLayerItemStruct item;

        QSqlRecord record = query.record();
        item.guid = record.value("guid").toString();
        item.link = record.value("link").toString();
        item.title = record.value("title").toString();
        item.description = record.value("description").toString();
        item.date = record.value("date").toDateTime();
        item.publisher = record.value("publisher").toString();
        item.pictures = record.value("pictures").toString().split(", ", QString::SkipEmptyParts);
        item.thumbnails = record.value("thumbnails").toString().split(", ", QString::SkipEmptyParts);
        item.type = record.value("type").toInt();
        item.eventId = record.value("eventId").toString();

        result << item;
    }

    return result;
}

QList<ApiLayerItemStruct> CacheDatabase::fetchEvents(const QString &eventId, int limit) const
{
    QList<ApiLayerItemStruct> result;

    QSqlQuery query(p->db);
    query.prepare("SELECT * FROM post WHERE type=:type "
                  "AND eventId=:eventId ORDER BY date DESC LIMIT :limit");
    query.bindValue(":eventId", eventId);
    query.bindValue(":limit", limit);
    query.bindValue(":type", ApiLayerItemStruct::EventPost);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << query.lastError().text();

    while(query.next())
    {
        ApiLayerItemStruct item;

        QSqlRecord record = query.record();
        item.guid = record.value("guid").toString();
        item.link = record.value("link").toString();
        item.title = record.value("title").toString();
        item.description = record.value("description").toString();
        item.date = record.value("date").toDateTime();
        item.publisher = record.value("publisher").toString();
        item.pictures = record.value("pictures").toString().split(", ", QString::SkipEmptyParts);
        item.thumbnails = record.value("thumbnails").toString().split(", ", QString::SkipEmptyParts);
        item.type = record.value("type").toInt();
        item.eventId = record.value("eventId").toString();

        result << item;
    }

    return result;
}

QList<ApiLayerItemStruct> CacheDatabase::readUpdates(int offset, int limit, int type) const
{
    QList<ApiLayerItemStruct> result;

    QSqlQuery query(p->db);
    query.prepare("SELECT * FROM post WHERE type=:type "
                  "ORDER BY date DESC "
                  "LIMIT :limit "
                  "OFFSET :offset");
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);
    query.bindValue(":type", type);
    if(!query.exec())
        qDebug() << __PRETTY_FUNCTION__ << query.lastError().text();

    while(query.next())
    {
        ApiLayerItemStruct item;

        QSqlRecord record = query.record();
        item.guid = record.value("guid").toString();
        item.link = record.value("link").toString();
        item.title = record.value("title").toString();
        item.description = record.value("description").toString();
        item.date = record.value("date").toDateTime();
        item.publisher = record.value("publisher").toString();
        item.pictures = record.value("pictures").toString().split(", ", QString::SkipEmptyParts);
        item.thumbnails = record.value("thumbnails").toString().split(", ", QString::SkipEmptyParts);
        item.type = record.value("type").toInt();
        item.eventId = record.value("eventId").toString();

        result << item;
    }

    return result;
}

void CacheDatabase::begin()
{
    QSqlQuery query(p->db);
    query.prepare("BEGIN");
    query.exec();
}

void CacheDatabase::commit()
{
    QSqlQuery query(p->db);
    query.prepare("COMMIT");
    query.exec();
}

void CacheDatabase::vacuum()
{
    QSqlQuery query(p->db);
    query.prepare("VACUUM");
    query.exec();
}

CacheDatabase::~CacheDatabase()
{
    delete p;
}

