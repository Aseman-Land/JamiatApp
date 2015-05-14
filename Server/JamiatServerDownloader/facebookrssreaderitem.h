#ifndef FACEBOOKRSSREADERITEM_H
#define FACEBOOKRSSREADERITEM_H

#include <QString>
#include <QDateTime>
#include <QObject>
#include <QStringList>

class FacebookRssReaderItem
{
    friend class FacebookRssDownloader;
    friend class FacebookRssImageDownloader;

public:
    enum PostType {
        NormalPost = 1,
        EventPost = 2,
        ReportPost = 3
    };

private:
    FacebookRssReaderItem();
    ~FacebookRssReaderItem();

    void setGuid(const QString &guid) {
        _guid = guid;
    }

    void setTitle(const QString &title) {
        _title = title;
    }

    void setLink(const QString &link) {
        _link = link;
    }

    void setDescription(const QString &description) {
        _description = description;
    }

    void setHtml(const QString &html) {
        _html = html;
    }

    void setPubDate(const QDateTime &pubDate) {
        _pubDate = pubDate;
    }

    void setAuthor(const QString &author) {
        _author = author;
    }

    void setCreator(const QString &creator) {
        _creator = creator;
    }

    void setImages(const QStringList &images) {
        _images = images;
    }

    void setLinks(const QStringList & links) {
        _links = links;
    }

    void setThumbnails(const QStringList &thumbnails) {
        _thumbnails = thumbnails;
    }

    void setEventId(const QString &eventId) {
        _eventId = eventId;
    }

    void setReportId(const QString &reportId) {
        _reportId = reportId;
    }

    void setType(int type) {
        _type = type;
    }

public:
    QString guid() const {
        return _guid;
    }

    QString title() const {
        return _title;
    }

    QString link() const {
        return _link;
    }

    QString description() const {
        return _description;
    }

    QString html() const {
        return _html;
    }

    QDateTime pubDate() const {
        return _pubDate;
    }

    QString author() const {
        return _author;
    }

    QString creator() const {
        return _creator;
    }

    QStringList images() const {
        return _images;
    }

    QStringList thumbnails() const {
        return _thumbnails;
    }

    QStringList links() const {
        return _links;
    }

    QString eventId() const {
        return _eventId;
    }

    QString reportId() const {
        return _reportId;
    }

    int type() const {
        return _type;
    }

private:
    QString _guid;
    QString _title;
    QString _link;
    QString _description;
    QString _html;
    QString _eventId;
    QString _reportId;
    QDateTime _pubDate;
    QString _author;
    QString _creator;
    QStringList _images;
    QStringList _thumbnails;
    QStringList _links;
    int _type;
};

#endif // FACEBOOKRSSREADERITEM_H
