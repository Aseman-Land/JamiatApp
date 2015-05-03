/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

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

#ifndef RSSREADER_H
#define RSSREADER_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QVariant>

class QDomElement;
class RssReaderPrivate;
class RssReader : public QObject
{
    Q_OBJECT
public:
    RssReader(QObject *parent = 0);
    ~RssReader();

    void setFilePath(const QString &filePath);
    QString filePath() const;

    QVariantList value() const;

public slots:
    void refresh();

signals:
    void filePathChanged();
    void valueChanged();

private:
    QVariantMap parseItemElement(const QDomElement &element);

private:
    RssReaderPrivate *p;
};

#endif // RSSREADER_H
