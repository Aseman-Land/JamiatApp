#ifndef CACHEDATABASE_H
#define CACHEDATABASE_H

#include <QObject>

class ApiLayer_ItemStruct;
typedef ApiLayer_ItemStruct ApiLayerItemStruct;

class CacheDatabasePrivate;
class CacheDatabase : public QObject
{
    Q_OBJECT
public:
    CacheDatabase(QObject *parent = 0);
    ~CacheDatabase();

    void write(const QList<ApiLayerItemStruct> &list);

    QList<ApiLayerItemStruct> readUpdates(int offset = 0, int limit = 10) const;
    QList<ApiLayerItemStruct> readEvents(int limit = 10) const;
    QList<ApiLayerItemStruct> fetchEvents(const QString &eventId, int limit = 10) const;
    QList<ApiLayerItemStruct> readReports(int limit = 10) const;

private:
    QList<ApiLayerItemStruct> readUpdates(int offset, int limit, int type) const;

    void begin();
    void commit();
    void vacuum();

private:
    CacheDatabasePrivate *p;
};

#endif // CACHEDATABASE_H
