#ifndef JAMIATTCPSERVER_H
#define JAMIATTCPSERVER_H

#include <QTcpServer>

class JamiatTcpServerPrivate;
class JamiatTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    JamiatTcpServer(QObject *parent = 0);
    ~JamiatTcpServer();

signals:
    void newConnection(qintptr handle);

protected:
    void incomingConnection(qintptr handle);

private:
    JamiatTcpServerPrivate *p;
};

#endif // JAMIATTCPSERVER_H
