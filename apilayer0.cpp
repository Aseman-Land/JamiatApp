#include "apilayer0.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QPointer>
#include <QTimerEvent>
#include <QQueue>

class ApiLayer0Private
{
public:
    qint64 id_counter;
    QPointer<QTcpSocket> socket;

    QHash<qint64, int> waitingList;
    QQueue<QByteArray> queue;
};

ApiLayer0::ApiLayer0(QObject *parent) :
    QObject(parent)
{
    p = new ApiLayer0Private;
    p->id_counter = 10000;
    initSocket();
}

void ApiLayer0::initSocket()
{
    if(p->socket)
        delete p->socket;

    p->socket = new QTcpSocket(this);
//    p->socket->connectToHost("127.0.0.1", 34946);
    p->socket->connectToHost("aseman.land", 34946);

    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    connect(p->socket, SIGNAL(readyRead()), SLOT(onReadyRead()) );
    connect(p->socket, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(error_prv(QAbstractSocket::SocketError)), Qt::QueuedConnection);
    connect(p->socket, SIGNAL(connected()), SLOT(writeQueue())  );
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

    startTimeOut(p->id_counter);
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

    startTimeOut(p->id_counter);
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

    startTimeOut(p->id_counter);
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

    startTimeOut(p->id_counter);
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

    startTimeOut(p->id_counter);
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
    checkTimeOut(id);

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
    checkTimeOut(id);

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
    checkTimeOut(id);

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
    checkTimeOut(id);

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
    checkTimeOut(id);

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

void ApiLayer0::error_prv(QAbstractSocket::SocketError socketError)
{
    QString text;
    switch(static_cast<int>(socketError))
    {
    case QAbstractSocket::ConnectionRefusedError:
        text = "The connection was refused by the peer (or timed out).";
        initSocket();
        break;

    case QAbstractSocket::RemoteHostClosedError:
        text = "The remote host closed the connection.";
        initSocket();
        break;

    case QAbstractSocket::HostNotFoundError:
        text = "The host address was not found.";
        break;

    case QAbstractSocket::SocketAccessError:
        text = "The socket operation failed because the application lacked the required privileges.";
        break;

    case QAbstractSocket::SocketResourceError:
        text = "The local system ran out of resources.";
        break;

    case QAbstractSocket::SocketTimeoutError:
        text = "The socket operation timed out.";
        break;

    case QAbstractSocket::DatagramTooLargeError:
        text = "The datagram was larger than the operating system's limit.";
        break;

    case QAbstractSocket::NetworkError:
        text = "An error occurred with the network.";
        initSocket();
        break;

    case QAbstractSocket::AddressInUseError:
        text = "The bound address is already in use and was set to be exclusive.";
        break;

    case QAbstractSocket::SocketAddressNotAvailableError:
        text = "The bound address does not belong to the host.";
        break;

    case QAbstractSocket::UnsupportedSocketOperationError:
        text = "The requested socket operation is not supported by the local operating system.";
        break;

    case QAbstractSocket::UnfinishedSocketOperationError:
        text = "The last operation attempted has not finished yet (still in progress in the background).";
        break;

    case QAbstractSocket::ProxyAuthenticationRequiredError:
        text = "The socket is using a proxy, and the proxy requires authentication.";
        break;

    case QAbstractSocket::SslHandshakeFailedError:
        text = "The SSL/TLS handshake failed, so the connection was closed";
        break;

    case QAbstractSocket::ProxyConnectionRefusedError:
        text = "Could not contact the proxy server because the connection to that server was denied.";
        break;

    case QAbstractSocket::ProxyConnectionClosedError:
        text = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
        break;

    case QAbstractSocket::ProxyConnectionTimeoutError:
        text = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
        break;

    case QAbstractSocket::ProxyNotFoundError:
        text = "The proxy address was not found.";
        break;

    case QAbstractSocket::ProxyProtocolError:
        text = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.";
        break;

    case QAbstractSocket::OperationError:
        text = "An operation was attempted while the socket was in a state that did not permit it.";
        break;

    case QAbstractSocket::SslInternalError:
        text = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.";
        break;

    case QAbstractSocket::SslInvalidUserDataError:
        text = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.";
        break;

    case QAbstractSocket::TemporaryError:
        text = "A temporary error occurred.";
        break;

    case QAbstractSocket::UnknownSocketError:
        text = "An unidentified error occurred.";
        break;
    }

    QHashIterator<qint64, int> i(p->waitingList);
    while(i.hasNext())
    {
        i.next();
        killTimer(i.value());
    }

    p->waitingList.clear();
    emit error(text);
}

void ApiLayer0::writeQueue()
{
    while(!p->queue.isEmpty())
        write(p->queue.takeFirst());
}

void ApiLayer0::write(QByteArray data)
{
    if(p->socket->state() == QAbstractSocket::ConnectedState)
    {
        p->socket->write(data.replace("\n", "\r\t\t\r") + "\n");
        p->socket->flush();
    }
    else
    {
        p->queue.append(data);
    }
}

QByteArray ApiLayer0::read(qint64 maxlen)
{
    QByteArray data = p->socket->readLine(maxlen);
    data.replace("\n", "");
    data.replace("\r\t\t\r", "\n");

    return data;
}

void ApiLayer0::startTimeOut(qint64 id)
{
    const int timerId = startTimer(10000);
    p->waitingList.insert(id, timerId);
}

void ApiLayer0::checkTimeOut(qint64 id)
{
    if(!p->waitingList.contains(id))
        return;

    const int timerId = p->waitingList.value(id);
    killTimer(timerId);
    p->waitingList.remove(id);
}

void ApiLayer0::timerEvent(QTimerEvent *e)
{
    qint64 reqId = p->waitingList.key(e->timerId());
    if(reqId)
    {
        p->waitingList.remove(reqId);
        killTimer(e->timerId());

        emit error("The connection was refused by the peer (or timed out).");
    }
}

ApiLayer0::~ApiLayer0()
{
    delete p;
}
