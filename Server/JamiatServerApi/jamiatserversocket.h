#ifndef JAMIATSERVERSOCKET_H
#define JAMIATSERVERSOCKET_H

#include <QObject>
#include <QVariantMap>
#include <QHostAddress>

class QDataStream;
class JamiatServerSocketPrivate;
class JamiatServerSocket : public QObject
{
    Q_OBJECT
    friend class CylinderItem;

private:
    JamiatServerSocket(qintptr handle, const QString &connection, QObject *parent = 0);
    ~JamiatServerSocket();

public:
    qintptr socketDescriptor() const;

public slots:
    void start();

signals:
    void disconnected();

private slots:
    void readyRead();
    void blockThisShit(const QHostAddress &host);

private:
    QByteArray callServiceApi0(qint64 id, const QByteArray &data);
    void init_db();
    void init_apis();

    void write(QByteArray data);
    QByteArray read(qint64 maxlen = 0);

private:
    JamiatServerSocketPrivate *p;
};

#endif // JAMIATSERVERSOCKET_H
