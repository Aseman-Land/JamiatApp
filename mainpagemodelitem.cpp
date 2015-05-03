#include "mainpagemodelitem.h"

class MainPageModelItemPrivate
{
public:
    QString guid;
    QStringList images;
    QString body;
    QColor postColor;
    QString postTitle;
    QString dateText;
    QString link;
    QString eventId;
};

MainPageModelItem::MainPageModelItem(QObject *parent) :
    AsemanColorfullListItem(parent)
{
    p = new MainPageModelItemPrivate;
}

void MainPageModelItem::setGuid(const QString &guid)
{
    if(p->guid == guid)
        return;

    p->guid = guid;
    emit guidChanged();
}

QString MainPageModelItem::guid() const
{
    return p->guid;
}

void MainPageModelItem::setBody(const QString &body)
{
    if( p->body == body )
        return;

    p->body = body;
    emit bodyChanged();
}

QString MainPageModelItem::body() const
{
    return p->body;
}

void MainPageModelItem::setImages(const QStringList &images)
{
    if( p->images == images )
        return;

    p->images = images;
    emit imagesChanged();
}

QStringList MainPageModelItem::images() const
{
    return p->images;
}

void MainPageModelItem::setPostTitle(const QString &title)
{
    if( p->postTitle == title )
        return;

    p->postTitle = title;
    emit postTitleChanged();
}

QString MainPageModelItem::postTitle() const
{
    return p->postTitle;
}

void MainPageModelItem::setDateText(const QString &date)
{
    if(p->dateText == date)
        return;

    p->dateText = date;
    emit dateTextChanged();
}

QString MainPageModelItem::dateText() const
{
    return p->dateText;
}

void MainPageModelItem::setLink(const QString &link)
{
    if(p->link == link)
        return;

    p->link = link;
    emit linkChanged();
}

QString MainPageModelItem::link() const
{
    return p->link;
}

void MainPageModelItem::setEventId(const QString &id)
{
    if(p->eventId == id)
        return;

    p->eventId = id;
    emit eventIdChanged();
}

QString MainPageModelItem::eventId()
{
    return p->eventId;
}

MainPageModelItem::~MainPageModelItem()
{
    delete p;
}

