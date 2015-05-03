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

#ifndef JAMIATAPP_H
#define JAMIATAPP_H

#include <QObject>
#include <QSize>
#include <QStringList>

class JamiatAppPrivate;
class JamiatApp : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList languages READ languages NOTIFY fakeSignal)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    JamiatApp(QObject *parent = 0);
    ~JamiatApp();

    Q_INVOKABLE QSize imageSize( const QString & path );
    Q_INVOKABLE QString fixPath( const QString & path );

    QStringList languages();

    void setLanguage( const QString & lang );
    QString language() const;

    Q_INVOKABLE QString textToHtml(const QString &text);

public slots:
    void start();
    void close();

signals:
    void backRequest();
    void fakeSignal();
    void languageChanged();
    void languageDirectionChanged();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    void init_languages();

private:
    JamiatAppPrivate *p;
};

#endif // JAMIATAPP_H
