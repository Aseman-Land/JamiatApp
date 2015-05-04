#ifndef APILAYER0_H
#define APILAYER0_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QTcpSocket>

class ApiLayer0_ItemStruct
{
public:
    enum PostType {
        NormalPost = 1,
        EventPost = 2
    };

    QString guid;
    QString link;
    QString title;
    QString description;
    QDateTime date;
    QString publisher;
    QStringList pictures;
    QStringList thumbnails;
    QString eventId;
    int type;
};

class ApiLayer0Private;
class ApiLayer0 : public QObject
{
    Q_OBJECT

public:
    enum StructRoles {
        UpdateRequestStruct = 0x1a23cc,
        UpdateStruct = 0x288d40,
        FullPostRequestStruct = 0x3166e0,
        FullPostStruct = 0x2b3428,
        SearchRequestStruct = 0x61b74b,
        SearchStruct = 0x9cfbff,
        LastEventsRequestStruct = 0x4d738c,
        LastEventsStruct =  0x798c49,
        FetchEventsRequestStruct = 0x909db7,
        FetchEventsStruct =  0xa7b792
    };

    enum ServicesRoles {
        ApiId = 0x2e3484,
        UpdateService = 0x24ca5c,
        FullPostService = 0x2f0cec,
        SearchService = 0x9ac6ff,
        LastEventsService = 0x44b493,
        FetchEventsService = 0x4a668f
    };

    enum PostType {
        NormalPost = 1,
        EventPost = 2
    };

    ApiLayer0(QObject *parent = 0);
    ~ApiLayer0();

    qint64 updateRequest(int offset = 0, int limit = 20);
    qint64 fullPostRequest(const QString &guid);
    qint64 searchRequest(const QString &keyword, int offset = 0, int limit = 20);
    qint64 lastEventsRequest(const QString &eventId = QString(), int offset = 0, int limit = 20);
    qint64 fetchEventsRequest(int offset = 0, int limit = 20);

signals:
    void updateRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void fullPostRequestAnswer(qint64 id, const ApiLayer0_ItemStruct &item);
    void searchRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void lastEventsRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void fetchEventsRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void error(const QString &text);

private slots:
    void onReadyRead();
    void onUpdateRequestAnswer(QByteArray data);
    void onFullPostRequestAnswer(QByteArray data);
    void onSearchRequestAnswer(QByteArray data);
    void onLastEventsRequestAnswer(QByteArray data);
    void onFetchEventsRequestAnswer(QByteArray data);

    void error_prv(QAbstractSocket::SocketError socketError);
    void writeQueue();

private:
    void write(QByteArray data);
    QByteArray read(qint64 maxlen = 0);
    QTcpSocket *getSocket();

    void startTimeOut(qint64 id);
    void checkTimeOut(qint64 id);

    void initSocket();

protected:
    void timerEvent(QTimerEvent *e);

private:
    ApiLayer0Private *p;
};

#endif // APILAYER0_H
