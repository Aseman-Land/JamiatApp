#ifndef JAMIATSERVERTESTER_H
#define JAMIATSERVERTESTER_H

#include <QObject>

class JamiatServerTesterPrivate;
class JamiatServerTester : public QObject
{
    Q_OBJECT
public:
    JamiatServerTester(QObject *parent = 0);
    ~JamiatServerTester();

public slots:
    void start();

private slots:
    void create();

private:
    JamiatServerTesterPrivate *p;
};

#endif // JAMIATSERVERTESTER_H
