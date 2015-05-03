#ifndef JAMIATSERVER_H
#define JAMIATSERVER_H

#include <QObject>

class JamiatServerPrivate;
class JamiatServer : public QObject
{
    Q_OBJECT
public:
    JamiatServer(QObject *parent = 0);
    ~JamiatServer();

public slots:
    void start();
    void stop();

private slots:
    void newConnection(qintptr handle);

private:
    JamiatServerPrivate *p;
};

#endif // JAMIATSERVER_H
