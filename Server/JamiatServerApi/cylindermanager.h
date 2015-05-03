#ifndef CYLINDERMANAGER_H
#define CYLINDERMANAGER_H

#include <QObject>

class CylinderItem;
class CylinderManagerPrivate;
class CylinderManager : public QObject
{
    Q_OBJECT
public:
    CylinderManager(QObject *parent = 0);
    ~CylinderManager();

    void setMaximumCylinder(int max);
    int maximumCylinder() const;

public slots:
    void handleSocket(qintptr handle);
    void cleanUp();

signals:
    void maximumCylinderChanged();

private:
    CylinderItem *minimum(int count = 1) const;
    CylinderItem *maximum(int count = 1) const;
    CylinderItem *allocateSocket();

private:
    CylinderManagerPrivate *p;
};

Q_DECLARE_METATYPE(qintptr)

#endif // CYLINDERMANAGER_H
