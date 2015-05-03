#ifndef JAMIATDATABASE_H
#define JAMIATDATABASE_H

#include <QObject>

class FacebookRssReaderItem;
class JamiatDatabasePrivate;
class JamiatDatabase : public QObject
{
    Q_OBJECT
public:
    JamiatDatabase(QObject *parent = 0);
    ~JamiatDatabase();

    void insert(FacebookRssReaderItem *item);
    bool contains(const QString &guid);

private:
    void init_db();
    void init_buffer();

private:
    JamiatDatabasePrivate *p;
};

#endif // JAMIATDATABASE_H
