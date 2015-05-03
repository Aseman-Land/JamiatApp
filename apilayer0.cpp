#include "apilayer0.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QPointer>

class ApiLayer0Private
{
public:
    qint64 id_counter;
    QPointer<QTcpSocket> socket;
};

ApiLayer0::ApiLayer0(QObject *parent) :
    QObject(parent)
{
    p = new ApiLayer0Private;
    p->id_counter = 10000;

    p->socket = new QTcpSocket(this);
//    p->socket->connectToHost("127.0.0.1", 34946);
    p->socket->connectToHost("aseman.land", 34946);
    p->socket->waitForConnected();

    connect(p->socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

qint64 ApiLayer0::updateRequest(int offset, int limit)
{
    QDateTime fromTime = QDateTime::currentDateTime();
    fromTime = fromTime.addDays(1);

    p->id_counter++;

    QByteArray structData;
    QDataStream structStream(&structData, QIODevice::WriteOnly);
    structStream << static_cast<int>(UpdateRequestStruct);
    structStream << fromTime.toMSecsSinceEpoch();
    structStream << offset;
    structStream << limit;
    structStream << p->id_counter;

    QByteArray topData;
    QDataStream topStream(&topData, QIODevice::WriteOnly);
    topStream << static_cast<int>(ApiId);
    topStream << static_cast<int>(UpdateService);
    topStream << structData;

    write(topData);
    return p->id_counter;
}

qint64 ApiLayer0::fullPostRequest(const QString &guid)
{
    p->id_counter++;

    QByteArray structData;
    QDataStream structStream(&structData, QIODevice::WriteOnly);
    structStream << static_cast<int>(FullPostRequestStruct);
    structStream << guid;
    structStream << p->id_counter;

    QByteArray topData;
    QDataStream topStream(&topData, QIODevice::WriteOnly);
    topStream << static_cast<int>(ApiId);
    topStream << static_cast<int>(FullPostService);
    topStream << structData;

    write(topData);
    return p->id_counter;
}

qint64 ApiLayer0::searchRequest(const QString &keyword, int offset, int limit)
{
    p->id_counter++;

    QByteArray structData;
    QDataStream structStream(&structData, QIODevice::WriteOnly);
    structStream << static_cast<int>(SearchRequestStruct);
    structStream << offset;
    structStream << limit;
    structStream << keyword;
    structStream << p->id_counter;

    QByteArray topData;
    QDataStream topStream(&topData, QIODevice::WriteOnly);
    topStream << static_cast<int>(ApiId);
    topStream << static_cast<int>(SearchService);
    topStream << structData;

    write(topData);
    return p->id_counter;
}

qint64 ApiLayer0::lastEventsRequest(const QString &eventId, int offset, int limit)
{
    p->id_counter++;

    QByteArray structData;
    QDataStream structStream(&structData, QIODevice::WriteOnly);
    structStream << static_cast<int>(LastEventsRequestStruct);
    structStream << eventId;
    structStream << offset;
    structStream << limit;
    structStream << p->id_counter;

    QByteArray topData;
    QDataStream topStream(&topData, QIODevice::WriteOnly);
    topStream << static_cast<int>(ApiId);
    topStream << static_cast<int>(LastEventsService);
    topStream << structData;

    write(topData);
    return p->id_counter;
}

qint64 ApiLayer0::fetchEventsRequest(int offset, int limit)
{
    QDateTime fromTime = QDateTime::currentDateTime();
    fromTime = fromTime.addDays(1);

    p->id_counter++;

    QByteArray structData;
    QDataStream structStream(&structData, QIODevice::WriteOnly);
    structStream << static_cast<int>(FetchEventsRequestStruct);
    structStream << fromTime.toMSecsSinceEpoch();
    structStream << offset;
    structStream << limit;
    structStream << p->id_counter;

    QByteArray topData;
    QDataStream topStream(&topData, QIODevice::WriteOnly);
    topStream << static_cast<int>(ApiId);
    topStream << static_cast<int>(FetchEventsService);
    topStream << structData;

    write(topData);
    return p->id_counter;
}

void ApiLayer0::onReadyRead()
{
    while(p->socket->canReadLine())
    {
        QByteArray data = read();
        QDataStream stream(&data, QIODevice::ReadOnly);

        qint64 serviceId = 0;
        stream >> serviceId;
        if(stream.atEnd())
            return;

        QByteArray serviceData;
        stream >> serviceData;

        switch(serviceId)
        {
        case UpdateService:
            onUpdateRequestAnswer(serviceData);
            break;

        case FullPostService:
            onFullPostRequestAnswer(serviceData);
            break;

        case SearchService:
            onSearchRequestAnswer(serviceData);
            break;

        case LastEventsService:
            onLastEventsRequestAnswer(serviceData);
            break;

        case FetchEventsService:
            onFetchEventsRequestAnswer(serviceData);
            break;
        }
    }
}

void ApiLayer0::onUpdateRequestAnswer(QByteArray data)
{
    QList<ApiLayer0_ItemStruct> result;
    int structId = 0;
    qint64 id = 0;
    int count = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> structId;
    if(structId != UpdateStruct)
        return;

    stream >> id;
    if(stream.atEnd())
        return;

    stream >> count;

    for(int i=0; i<count; i++)
    {
        if(stream.atEnd())
            return;

        ApiLayer0_ItemStruct unit;
        stream >> unit.guid;
        stream >> unit.link;
        stream >> unit.title;
        stream >> unit.description;
        stream >> unit.date;
        stream >> unit.publisher;
        stream >> unit.pictures;
        stream >> unit.thumbnails;
        stream >> unit.eventId;
        stream >> unit.type;

        if(unit.title.isEmpty() && unit.description.isEmpty())
            continue;

        result << unit;
    }

    emit updateRequestAnswer(id, result);
}

void ApiLayer0::onFullPostRequestAnswer(QByteArray data)
{
    ApiLayer0_ItemStruct result;
    int structId = 0;
    qint64 id = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> structId;
    if(structId != FullPostStruct)
        return;

    stream >> id;
    if(stream.atEnd())
        return;

    stream >> result.guid;
    stream >> result.link;
    stream >> result.title;
    stream >> result.description;
    stream >> result.date;
    stream >> result.publisher;
    stream >> result.pictures;
    stream >> result.thumbnails;
    stream >> result.eventId;
    stream >> result.type;

    emit fullPostRequestAnswer(id, result);
}

void ApiLayer0::onSearchRequestAnswer(QByteArray data)
{
    QList<ApiLayer0_ItemStruct> result;
    int structId = 0;
    qint64 id = 0;
    int count = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> structId;
    if(structId != SearchStruct)
        return;

    stream >> id;
    if(stream.atEnd())
        return;

    stream >> count;

    for(int i=0; i<count; i++)
    {
        if(stream.atEnd())
            return;

        ApiLayer0_ItemStruct unit;
        stream >> unit.guid;
        stream >> unit.link;
        stream >> unit.title;
        stream >> unit.description;
        stream >> unit.date;
        stream >> unit.publisher;
        stream >> unit.pictures;
        stream >> unit.thumbnails;
        stream >> unit.eventId;
        stream >> unit.type;

        result << unit;
    }

    emit searchRequestAnswer(id, result);
}

void ApiLayer0::onLastEventsRequestAnswer(QByteArray data)
{
    QList<ApiLayer0_ItemStruct> result;
    int structId = 0;
    qint64 id = 0;
    int count = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> structId;
    if(structId != LastEventsStruct)
        return;

    stream >> id;
    if(stream.atEnd())
        return;

    stream >> count;

    for(int i=0; i<count; i++)
    {
        if(stream.atEnd())
            return;

        ApiLayer0_ItemStruct unit;
        stream >> unit.guid;
        stream >> unit.link;
        stream >> unit.title;
        stream >> unit.description;
        stream >> unit.date;
        stream >> unit.publisher;
        stream >> unit.pictures;
        stream >> unit.thumbnails;
        stream >> unit.eventId;
        stream >> unit.type;

        result << unit;
    }

    emit lastEventsRequestAnswer(id, result);
}

void ApiLayer0::onFetchEventsRequestAnswer(QByteArray data)
{
    QList<ApiLayer0_ItemStruct> result;
    int structId = 0;
    qint64 id = 0;
    int count = 0;

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> structId;
    if(structId != FetchEventsStruct)
        return;

    stream >> id;
    if(stream.atEnd())
        return;

    stream >> count;

    for(int i=0; i<count; i++)
    {
        if(stream.atEnd())
            return;

        ApiLayer0_ItemStruct unit;
        stream >> unit.guid;
        stream >> unit.link;
        stream >> unit.title;
        stream >> unit.description;
        stream >> unit.date;
        stream >> unit.publisher;
        stream >> unit.pictures;
        stream >> unit.thumbnails;
        stream >> unit.eventId;
        stream >> unit.type;

        result << unit;
    }

    emit fetchEventsRequestAnswer(id, result);
}

void ApiLayer0::write(QByteArray data)
{
    p->socket->write(data.replace("\n", "\r") + "\n");
    p->socket->flush();
}

QByteArray ApiLayer0::read(qint64 maxlen)
{
    QByteArray data = p->socket->readLine(maxlen);
    data.replace("\n", "");
    data.replace("\r", "\n");

    return data;
}

ApiLayer0::~ApiLayer0()
{
    delete p;
}
