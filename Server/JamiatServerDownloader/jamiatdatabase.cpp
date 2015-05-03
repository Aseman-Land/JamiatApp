#define DB_CONNECTION_NAME "sql_sosa_connection"

#include "jamiatdatabase.h"
#include "facebookrssreaderitem.h"

#include <QSqlDatabase>
#include <QSettings>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMap>
#include <QSet>
#include <QDebug>

class JamiatDatabasePrivate
{
public:
    QSqlDatabase db;

    QMap<int, QString> map;
    QSet<QString> guids;
};

JamiatDatabase::JamiatDatabase(QObject *parent) :
    QObject(parent)
{
    p = new JamiatDatabasePrivate;
    init_db();
    init_buffer();
}

void JamiatDatabase::insert(FacebookRssReaderItem *item)
{
    if(p->guids.contains(item->guid()))
        return;

    const int postId = p->map.isEmpty()? 1 : p->map.lastKey()+1;

    QSqlQuery post_query(p->db);
    post_query.prepare("INSERT INTO post (id, guid, link, title, description, date, publisher, creator, type, eventId) VALUES (:id, :guid, :link, :title, :description, :date, :publisher, :creator, :type, :eventId);");
    post_query.bindValue(":id", postId);
    post_query.bindValue(":guid", item->guid());
    post_query.bindValue(":link", item->link());
    post_query.bindValue(":title", item->title());
    post_query.bindValue(":description", item->description());
    post_query.bindValue(":date", item->pubDate());
    post_query.bindValue(":publisher", item->author());
    post_query.bindValue(":creator", item->creator());
    post_query.bindValue(":type", item->type());
    post_query.bindValue(":eventId", item->eventId());
    if( !post_query.exec() )
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << post_query.lastError().text();

    const QStringList &images = item->images();
    const QStringList &thumbs = item->thumbnails();
    for(int i=0; i<images.length(); i++)
    {
        const QString &img = images[i];
        const QString &tmb = thumbs[i];
        const QString &id = img.mid(img.lastIndexOf("/")+1);

        QSqlQuery images_query(p->db);
        images_query.prepare("INSERT INTO image (id, post_id, picture, thumbnail) VALUES (:id, :post_id, :picture, :thumbnail);");
        images_query.bindValue(":id", id);
        images_query.bindValue(":post_id", postId);
        images_query.bindValue(":picture", img);
        images_query.bindValue(":thumbnail", tmb);
        if( !images_query.exec() )
            qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << images_query.lastError().text();
    }

    p->map[postId] = item->guid();
}

bool JamiatDatabase::contains(const QString &guid)
{
    return p->guids.contains(guid);
}

void JamiatDatabase::init_db()
{
    QString config_file = QCoreApplication::applicationDirPath() + "/config.ini";
    if(!QFileInfo::exists(config_file))
        config_file = "/etc/sosa/config.ini";

    Q_ASSERT_X(QFileInfo::exists(config_file), __PRETTY_FUNCTION__, "Cannot open config file.");

    QSettings settings(config_file, QSettings::IniFormat);

    p->db = QSqlDatabase::addDatabase("QMYSQL", DB_CONNECTION_NAME);
    p->db.setHostName( settings.value("database/hostname").toString() );
    p->db.setDatabaseName( settings.value("database/database").toString() );
    p->db.setUserName( settings.value("database/username").toString() );
    p->db.setPassword( settings.value("database/password").toString() );

    Q_ASSERT_X(p->db.open(), __PRETTY_FUNCTION__, "Cannot open database");

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
        QSqlQuery query(p->db);
        query.prepare(query_str);
        if(!query.exec())
            qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << query.lastError().text();
    }
}

void JamiatDatabase::init_buffer()
{
    QSqlQuery posts_query(p->db);
    posts_query.prepare("SELECT id, guid FROM post");
    posts_query.exec();
    if(!posts_query.exec())
        qDebug() << __PRETTY_FUNCTION__ << QDateTime::currentDateTime() << posts_query.lastError().text();

    while(posts_query.next())
    {
        QSqlRecord record = posts_query.record();
        const int id = record.value("id").toInt();
        const QString &guid = record.value("guid").toString();

        p->map[id] = guid;
        p->guids.insert(guid);
    }
}

JamiatDatabase::~JamiatDatabase()
{
    delete p;
}

