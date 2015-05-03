/*
    Copyright (C) 2014 Aseman Labs
    http://labs.aseman.org

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainpagemodel.h"
#include "apilayer0.h"
#include "mainpagemodelitem.h"
#include "cachedatabase.h"
#include "asemantools/asemancalendarconverter.h"

#include <QSet>
#include <QDebug>
#include <QTimer>

class MainPageModelPrivate
{
public:
    ApiLayerItemStruct lastEvent;
    QList<ApiLayerItemStruct> lastItems;
    QList<ApiLayerItemStruct> lastSearch;

    QList<MainPageModelItem*> items;
    ApiLayer0 *api;
    QString keyword;
    QString eventId;

    QTimer *refreshTimer;
    QTimer *garbageTimer;

    CacheDatabase *cache;
    AsemanCalendarConverter *calendar;

    QSet<qint64> waitingIds;
    QSet<QObject*> garbages;

    bool loadMoreState;
};

MainPageModel::MainPageModel(QObject *parent) :
    AsemanAbstractColorfullListModel(parent)
{
    p = new MainPageModelPrivate;
    p->loadMoreState = false;

    p->refreshTimer = new QTimer(this);
    p->refreshTimer->setInterval(1000);
    p->refreshTimer->setSingleShot(true);

    p->garbageTimer = new QTimer(this);
    p->garbageTimer->setInterval(2000);
    p->garbageTimer->setSingleShot(true);

    p->calendar = new AsemanCalendarConverter(this);
    p->calendar->setCalendar(AsemanCalendarConverterCore::Jalali);

    p->api = new ApiLayer0(this);
    p->cache = new CacheDatabase(this);

    connect(p->api, SIGNAL(lastEventsRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)),
            SLOT(lastEventsRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)));
    connect(p->api, SIGNAL(searchRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)),
            SLOT(searchRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)));
    connect(p->api, SIGNAL(updateRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)),
            SLOT(updateRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)));
    connect(p->api, SIGNAL(fetchEventsRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)),
            SLOT(fetchEventsRequestAnswer(qint64,QList<ApiLayer0_ItemStruct>)));

    connect(p->refreshTimer, SIGNAL(timeout()), SLOT(refresh()));
    connect(p->garbageTimer, SIGNAL(timeout()), SLOT(clearGarbages()));
}

int MainPageModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return row;
}

int MainPageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->items.count();
}

QVariant MainPageModel::data(const QModelIndex &index, int role) const
{
    const int key = id(index);
    QVariant res;
    if( key >= p->items.count() )
        return res;

    MainPageModelItem *item = p->items.value(key);

    switch( role )
    {
    case Qt::DisplayRole:
    case TitleRole:
        res = item->title();
        break;

    case ColorRole:
        res = item->color();
        break;

    case IsHeaderRole:
        res = item->isHeader();
        break;

    case ModelItemRole:
        res = QVariant::fromValue<MainPageModelItem*>(item);
        break;

    case BodyRole:
        res = item->body();
        break;

    case ImageRole:
        res = item->images();
        break;
    }

    return res;
}

QHash<qint32, QByteArray> MainPageModel::roleNames() const
{
    QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->unite( AsemanAbstractColorfullListModel::roleNames() );
    res->insert( BodyRole      , "body"      );
    res->insert( ImageRole     , "image"     );
    res->insert( PostTitleRole , "postTitle" );

    return *res;
}

void MainPageModel::setKeyword(const QString &kw)
{
    if(p->keyword == kw)
        return;

    p->keyword = kw;
    emit keywordChanged();

    p->refreshTimer->stop();
    if(p->keyword.isEmpty())
        refresh();
    else
        p->refreshTimer->start();

    refresh_prv( QList<ApiLayerItemStruct>() );
}

QString MainPageModel::keyword() const
{
    return p->keyword;
}

void MainPageModel::setEventId(const QString &eventId)
{
    if(p->eventId == eventId)
        return;

    p->eventId = eventId;
    emit eventIdChanged();

    refresh();
}

QString MainPageModel::eventId() const
{
    return p->eventId;
}

int MainPageModel::count() const
{
    return p->items.count();
}

bool MainPageModel::refreshing() const
{
    return !p->waitingIds.isEmpty();
}

AsemanColorfullListItem *MainPageModel::get(int row)
{
    if( row >= p->items.count() )
        return 0;

    return p->items.at(row);
}

void MainPageModel::refresh()
{
    if(!p->eventId.isEmpty())
    {
        refresh_prv( p->cache->fetchEvents(p->eventId, 10) );
        qint64 leId = p->api->lastEventsRequest(p->eventId, 0, 10);
        p->waitingIds.insert(leId);
    }
    else
    if(p->keyword.isEmpty())
    {
        qint64 leId = p->api->lastEventsRequest(QString(), 0, 1);
        qint64 udId = p->api->updateRequest(0, 10);

        p->waitingIds.insert(leId);
        p->waitingIds.insert(udId);
    }
    else
    {
        qint64 sId = p->api->searchRequest(p->keyword, 0, 10);
        p->waitingIds.insert(sId);
    }

    p->loadMoreState = false;
    emit refreshingChanged();
}

void MainPageModel::showEvents()
{
    refresh_prv(p->cache->readEvents(10));

    qint64 eId = p->api->fetchEventsRequest(0, 10);
    p->waitingIds.insert(eId);

    p->loadMoreState = false;
    emit refreshingChanged();
}

void MainPageModel::loadMore()
{
    if(refreshing())
        return;
    if(p->items.isEmpty() || p->items.count()<2)
        return;

    if(p->keyword.isEmpty())
    {
        qint64 udId = p->api->updateRequest(p->items.count()-1, 10);
        p->waitingIds.insert(udId);
    }
    else
    {
        qint64 sId = p->api->searchRequest(p->keyword, p->items.count(), 10);
        p->waitingIds.insert(sId);
    }

    p->loadMoreState = true;
    emit refreshingChanged();
}

void MainPageModel::readHistory()
{
    refresh_prv(p->cache->readUpdates());
}

void MainPageModel::refresh_prv(QList<ApiLayerItemStruct> items)
{
    items = makeUnique(items);

    QList<QString> guids;
    foreach(const ApiLayerItemStruct &item, items)
        guids << item.guid;

    QList<QString> pGuids;
    foreach(MainPageModelItem *item, p->items)
        pGuids << item->guid();

    for( int i=0 ; i<pGuids.count() ; i++ )
    {
        const QString &guid = pGuids.at(i);
        if( guids.contains(guid) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        insertGarbage(p->items.at(i));
        p->items.removeAt(i);
        pGuids.removeAt(i);
        i--;
        endRemoveRows();
    }


    QList<QString> temp_items = guids;
    for( int i=0 ; i<temp_items.count() ; i++ )
    {
        const QString &guid = temp_items.at(i);
        if( pGuids.contains(guid) )
            continue;

        temp_items.removeAt(i);
        i--;
    }
    while( pGuids != temp_items )
        for( int i=0 ; i<pGuids.count() ; i++ )
        {
            const QString &guid = pGuids.at(i);
            int nw = temp_items.indexOf(guid);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            pGuids.move(i, nw);
            p->items.move( i, nw );
            endMoveRows();
        }


    for( int i=0 ; i<guids.count() ; i++ )
    {
        const QString &guid = guids.at(i);
        if( pGuids.contains(guid) )
            continue;

        const ApiLayerItemStruct &item = items.at(i);
        bool isNews = item.type==ApiLayerItemStruct::NormalPost;

        MainPageModelItem *itemObj = new MainPageModelItem(this);
        itemObj->setGuid(item.guid);
        itemObj->setLink(item.link);
        itemObj->setTitle(isNews? tr("News") : (i==0? tr("Top Event") : tr("Event")) );
        itemObj->setBody(item.description);
        itemObj->setPostTitle(item.title);
        itemObj->setDateText( p->calendar->convertDateTimeToString(item.date) );
        itemObj->setColor(isNews? QColor("#3b5998") : QColor("#689f38"));
        itemObj->setImages(item.pictures);
        itemObj->setEventId(item.eventId);

        beginInsertRows(QModelIndex(), i, i );
        p->items.insert( i, itemObj );
        pGuids.insert(i, guid);
        endInsertRows();
    }

    QString lastTitle;
    foreach(MainPageModelItem *item, p->items)
    {
        item->setIsHeader(item->title() != lastTitle);
        lastTitle = item->title();
    }

    emit countChanged();
}

void MainPageModel::clearGarbages()
{
    foreach(QObject *obj, p->garbages)
        delete obj;

    p->garbages.clear();
}

QList<ApiLayerItemStruct> MainPageModel::makeUnique(QList<ApiLayerItemStruct> items)
{
    for(int i=0; i<items.length(); i++)
    {
        const ApiLayerItemStruct &item = items.at(i);
        for(int j=i+1; j<items.length(); j++)
            if(items.at(j).guid == item.guid)
            {
                items.removeAt(j);
                j--;
            }
    }

    return items;
}

void MainPageModel::insertGarbage(QObject *obj)
{
    p->garbages.insert(obj);
    p->garbageTimer->stop();
    p->garbageTimer->start();
}

void MainPageModel::updateRequestAnswer(qint64 id, const QList<ApiLayerItemStruct> &items)
{
    if(p->loadMoreState)
        p->lastItems << items;
    else
        p->lastItems = items;

    QList<ApiLayerItemStruct> list;
    if(!p->lastEvent.guid.isEmpty()) list << p->lastEvent;
    list << p->lastItems;

    refresh_prv(list);
    p->cache->write(list);

    p->waitingIds.remove(id);
    emit refreshingChanged();
}

void MainPageModel::searchRequestAnswer(qint64 id, const QList<ApiLayerItemStruct> &items)
{
    if(p->loadMoreState)
        p->lastSearch << items;
    else
        p->lastSearch = items;

    refresh_prv(p->lastSearch);
    p->cache->write(items);

    p->waitingIds.remove(id);
    emit refreshingChanged();
}

void MainPageModel::lastEventsRequestAnswer(qint64 id, const QList<ApiLayerItemStruct> &items)
{
    if(!p->eventId.isEmpty())
    {
        refresh_prv(items);
        p->cache->write(items);

        p->waitingIds.remove(id);
        emit refreshingChanged();
    }

    p->cache->write(items);
    if(!items.isEmpty())
        p->lastEvent = items.first();
    else
        p->lastEvent = ApiLayerItemStruct();

    if(p->lastItems.isEmpty())
    {
        p->waitingIds.remove(id);
        emit refreshingChanged();
        return;
    }

    QList<ApiLayerItemStruct> list;
    if(!p->lastEvent.guid.isEmpty()) list << p->lastEvent;
    list << p->lastItems;

    refresh_prv(list);

    p->waitingIds.remove(id);
    emit refreshingChanged();
}

void MainPageModel::fetchEventsRequestAnswer(qint64 id, const QList<ApiLayerItemStruct> &items)
{
    refresh_prv(items);
    p->cache->write(items);

    p->waitingIds.remove(id);
    emit refreshingChanged();
}

MainPageModel::~MainPageModel()
{
    delete p;
}

