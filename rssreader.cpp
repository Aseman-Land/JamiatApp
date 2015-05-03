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

#include "rssreader.h"

#include <QUrl>
#include <QFile>
#include <QStringList>
#include <QDomDocument>
#include <QTextDocument>
#include <QVariant>
#include <QDebug>

class RssReaderPrivate
{
public:
    QString filePath;
    QDomDocument dom;

    QVariantList value;
};

RssReader::RssReader(QObject *parent) :
    QObject(parent)
{
    p = new RssReaderPrivate;
}

void RssReader::setFilePath(const QString &filePath)
{
    if(p->filePath == filePath)
        return;

    p->filePath = filePath;
    emit filePathChanged();

    refresh();
}

QString RssReader::filePath() const
{
    return p->filePath;
}

QVariantList RssReader::value() const
{
    return p->value;
}

void RssReader::refresh()
{
    p->value.clear();
    if( !QFile::exists(p->filePath) )
        return;

    QFile file(p->filePath);
    if( !file.open(QFile::ReadOnly) )
        return;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!p->dom.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << QString("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr);
        return;
    }

    QDomElement root = p->dom.documentElement();
    if(root.tagName() != "rss")
    {
        qDebug() << QString("The file is not an TS file.");
        return;
    }
    else
    if(root.hasAttribute("version") && root.attribute("version") < "2.0")
    {
        qDebug() << QString("The file has old version.");
        return;
    }

    QDomElement child = root.firstChildElement("channel");
    if(child.isNull())
        return;

    QDomElement item = child.firstChildElement("item");
    while(!item.isNull())
    {
        p->value << QVariant::fromValue<QVariantMap>( parseItemElement(item) );
        item = item.nextSiblingElement("item");
    }
}

QVariantMap RssReader::parseItemElement(const QDomElement &element)
{
    QVariantMap result;

    QTextDocument doc;
    QDomElement child = element.firstChildElement();
    while (!child.isNull())
    {
        doc.setHtml(child.text());
        result[child.tagName()] = doc.toPlainText().trimmed();
        child = child.nextSiblingElement();
    }

    return result;
}

RssReader::~RssReader()
{
    delete p;
}
