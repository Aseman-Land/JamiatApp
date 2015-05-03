#ifndef MAINPAGEMODELITEM_H
#define MAINPAGEMODELITEM_H

#include "asemantools/asemanabstractcolorfulllistmodel.h"
#include <QObject>
#include <QStringList>

class MainPageModelItem: public AsemanColorfullListItem
{
    Q_OBJECT

    Q_PROPERTY(QString     guid      READ guid      NOTIFY guidChanged )
    Q_PROPERTY(QString     body      READ body      WRITE setBody      NOTIFY bodyChanged     )
    Q_PROPERTY(QStringList images    READ images    WRITE setImages    NOTIFY imagesChanged   )
    Q_PROPERTY(QString     postTitle READ postTitle WRITE setPostTitle NOTIFY postTitleChanged)
    Q_PROPERTY(QString     dateText  READ dateText  WRITE setDateText  NOTIFY dateTextChanged )
    Q_PROPERTY(QString     link      READ link      WRITE setLink      NOTIFY linkChanged     )
    Q_PROPERTY(QString     eventId   READ eventId   WRITE setEventId   NOTIFY eventIdChanged  )

public:
    MainPageModelItem( QObject *parent = 0 );
    ~MainPageModelItem();

    void setGuid( const QString & guid );
    QString guid() const;

    void setBody( const QString & body );
    QString body() const;

    void setImages( const QStringList & images );
    QStringList images() const;

    void setPostTitle( const QString & title );
    QString postTitle() const;

    void setDateText(const QString &date);
    QString dateText() const;

    void setLink(const QString &link);
    QString link() const;

    void setEventId(const QString &id);
    QString eventId();

signals:
    void guidChanged();
    void bodyChanged();
    void imagesChanged();
    void postTitleChanged();
    void dateTextChanged();
    void linkChanged();
    void eventIdChanged();

private:
    class MainPageModelItemPrivate *p;
};

#endif // MAINPAGEMODELITEM_H
