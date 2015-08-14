#ifndef JAMIATSERVERTESTERITEM_H
#define JAMIATSERVERTESTERITEM_H

#include "apilayer.h"
#include <QObject>

class JamiatServerTesterItemPrivate;
class JamiatServerTesterItem : public QObject
{
    Q_OBJECT
public:
    JamiatServerTesterItem(QObject *parent = 0);
    ~JamiatServerTesterItem();

public slots:
    void start();

private slots:
    void sendRequest();
    void updateRequestAnswer(qint64 id, const QList<ApiLayer_ItemStruct> &items);

private:
    JamiatServerTesterItemPrivate *p;
};

#endif // JAMIATSERVERTESTERITEM_H
