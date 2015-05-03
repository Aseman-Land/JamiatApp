#ifndef JAMIATANDROIDSERVICE_H
#define JAMIATANDROIDSERVICE_H

#include <QObject>

class JamiatAndroidServicePrivate;
class JamiatAndroidService : public QObject
{
    Q_OBJECT
public:
    JamiatAndroidService(QObject *parent = 0);
    ~JamiatAndroidService();

public slots:
    void start();

private:
    JamiatAndroidServicePrivate *p;
};

#endif // JAMIATANDROIDSERVICE_H
