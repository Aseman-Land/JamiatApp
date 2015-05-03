#ifndef CYLINDERITEM_H
#define CYLINDERITEM_H

#include <QObject>

class CylinderItemPrivate;
class CylinderItem : public QObject
{
    Q_OBJECT

    friend class CylinderManager;

private:
    CylinderItem(QObject *parent = 0);
    ~CylinderItem();

public:
    int count() const;

public slots:
    void handleSocket(qintptr handle);

private slots:
    void disconnected();

private:
    CylinderItemPrivate *p;
};

#endif // CYLINDERITEM_H
