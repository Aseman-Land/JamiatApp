#ifndef JAMIATSERVICESAPI1_H
#define JAMIATSERVICESAPI1_H

#include <QObject>

class JamiatServicesApi0Private;
class JamiatServicesApi1 : public QObject
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
        FetchEventsStruct =  0xa7b792,
        FetchReportsRequestStruct = 0xafb56d,
        FetchReportsStruct =  0xab72b5
    };

    enum ServicesRoles {
        ApiId = 0x5194a1,
        UpdateService = 0x24ca5c,
        FullPostService = 0x2f0cec,
        SearchService = 0x9ac6ff,
        LastEventsService = 0x44b493,
        FetchEventsService = 0x4a668f,
        FetchReportsService = 0x44b571
    };

    enum PostType {
        NormalPost = 1,
        EventPost = 2,
        ReportPost = 3
    };

    JamiatServicesApi1(const QString &connection, QObject *parent = 0);
    ~JamiatServicesApi1();

    QByteArray updateService(QByteArray data);
    QByteArray fullPostService(QByteArray data);
    QByteArray searchService(QByteArray data);
    QByteArray lastEventsService(QByteArray data);
    QByteArray fetchEventsService(QByteArray data);
    QByteArray fetchReportsService(QByteArray data);

private:
    JamiatServicesApi0Private *p;
};

#endif // JAMIATSERVICESAPI1_H
