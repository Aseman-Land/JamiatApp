#ifndef JAMIATSERVICESAPI0_H
#define JAMIATSERVICESAPI0_H

#include <QObject>

class JamiatServicesApi0Private;
class JamiatServicesApi0 : public QObject
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

    JamiatServicesApi0(const QString &connection, QObject *parent = 0);
    ~JamiatServicesApi0();

    QByteArray updateService(QByteArray data);
    QByteArray fullPostService(QByteArray data);
    QByteArray searchService(QByteArray data);
    QByteArray lastEventsService(QByteArray data);
    QByteArray fetchEventsService(QByteArray data);

private:
    JamiatServicesApi0Private *p;
};

#endif // JAMIATSERVICESAPI0_H
