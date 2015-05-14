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

#include "jamiatapp.h"
#include "mainpagemodel.h"
#include "asemantools/asemanapplication.h"
#include "asemantools/asemandevices.h"
#include "asemantools/asemanquickview.h"
#ifdef Q_OS_ANDROID
#include "asemantools/asemanjavalayer.h"
#endif

#include <QtQml>
#include <QPointer>
#include <QImageReader>
#include <QDebug>
#include <QTranslator>
#include <QGuiApplication>
#include <QLocale>
#include <QRegExp>
#include <QTextDocument>

class JamiatAppPrivate
{
public:
    QPointer<AsemanQuickView> viewer;
    bool close_blocker;

    QTranslator *translator;
    QString translationsPath;

    QHash<QString,QVariant> languages;
    QHash<QString,QLocale> locales;
    QString language;
};

JamiatApp::JamiatApp(QObject *parent) :
    QObject(parent)
{
    p = new JamiatAppPrivate;
    p->translator = new QTranslator(this);

#ifdef Q_OS_ANDROID
    p->close_blocker = true;
    p->translationsPath = "assets:/files/translations";
#else
    p->close_blocker = false;
    p->translationsPath = AsemanDevices::resourcePath() + "/files/translations/";
#endif

    qmlRegisterType<MainPageModel>("JamiatApp", 1,0, "MainPageModel");

    init_languages();
}

void JamiatApp::start()
{
    if( p->viewer )
        return;

    p->viewer = new AsemanQuickView(AsemanQuickView::AllExceptLogger);
    p->viewer->installEventFilter(this);
#ifdef Q_OS_ANDROID
    p->viewer->javaLayer()->startService();
#endif
    p->viewer->engine()->rootContext()->setContextProperty( "Jamiat", this );
    p->viewer->setSource(QUrl(QStringLiteral("qrc:/qml/JamiatApp/main.qml")));
    p->viewer->show();
}

void JamiatApp::close()
{
    p->close_blocker = false;
    p->viewer->close();
}

QSize JamiatApp::imageSize(const QString &pt)
{
    QString path = pt;
    if(path.left(AsemanDevices::localFilesPrePath().size()) == AsemanDevices::localFilesPrePath())
        path = path.mid(AsemanDevices::localFilesPrePath().size());

    QImageReader img(path);
    return img.size();
}

QString JamiatApp::fixPath(const QString &pt)
{
    QString path = pt;
    if(path.left(AsemanDevices::localFilesPrePath().size()) == AsemanDevices::localFilesPrePath())
        path = path.mid(AsemanDevices::localFilesPrePath().size());
    if( path.isEmpty() )
        return path;

    if( path.left(5) == "qrc:/" )
        return path.mid(3);
    else
    if( QFileInfo::exists(path) )
        return path;
    else
    if( QFileInfo::exists(":/"+path) )
        return ":/"+path;
    else
    if( QFileInfo::exists(":/qml/JamiatApp/"+path) )
        return ":/qml/JamiatApp/"+path;
    else
        return path;
}

QStringList JamiatApp::languages()
{
    QStringList res = p->languages.keys();
    res.sort();
    return res;
}

void JamiatApp::setLanguage(const QString &lang)
{
    if( p->language == lang )
        return;

    QGuiApplication::removeTranslator(p->translator);
    p->translator->load(p->languages.value(lang).toString(),"languages");
    QGuiApplication::installTranslator(p->translator);
    p->language = lang;

    AsemanApplication::settings()->setValue("General/Language",lang);

    emit languageChanged();
    emit languageDirectionChanged();
}

QString JamiatApp::language() const
{
    return p->language;
}

QString JamiatApp::textToHtml(const QString &text)
{
    QString res = text.toHtmlEscaped();

    QRegExp links_rxp("((?:(?:\\w\\S*\\/\\S*|\\/\\S+|\\:\\/)(?:\\/\\S*\\w|\\w))|(?:\\w+\\.(?:com|org|co|net)))");
    int pos = 0;
    while ((pos = links_rxp.indexIn(res, pos)) != -1)
    {
        QString link = links_rxp.cap(1);
        QString href = link;
        if(href.indexOf(QRegExp("\\w+\\:\\/\\/")) == -1)
            href = "http://" + href;

        QString atag = QString("<a href='%1'>%2</a>").arg(href,link);
        res.replace( pos, link.length(), atag );
        pos += atag.size();
    }

    res.replace("\n", "<br />");
    res.replace(QRegExp("\\=\\=\\=+"), "<br /> =========== <br />");
    return res;
}

QString JamiatApp::fixText(const QString &text)
{
    QString res = text;
    res.replace(QRegExp("\\=\\=\\=+"), "\n=========== \n");
    return res;
}

bool JamiatApp::eventFilter(QObject *o, QEvent *e)
{
    if( o == p->viewer )
    {
        switch( static_cast<int>(e->type()) )
        {
        case QEvent::Close:
            if( p->close_blocker )
            {
                static_cast<QCloseEvent*>(e)->ignore();
                emit backRequest();
            }
            else
            {
                static_cast<QCloseEvent*>(e)->accept();
            }

            return false;
            break;
        }
    }

    return QObject::eventFilter(o,e);
}

void JamiatApp::init_languages()
{
    QDir dir(p->translationsPath);
    QStringList languages = dir.entryList( QDir::Files );
    if( !languages.contains("lang-en.qm") )
        languages.prepend("lang-en.qm");

    for( int i=0 ; i<languages.size() ; i++ )
    {
        QString locale_str = languages[i];
        locale_str.truncate( locale_str.lastIndexOf('.') );
        locale_str.remove( 0, locale_str.indexOf('-') + 1 );

        QLocale locale(locale_str);

        QString  lang = QLocale::languageToString(locale.language());
        QVariant data = p->translationsPath + "/" + languages[i];

        p->languages.insert( lang, data );
        p->locales.insert( lang , locale );

        if( lang == AsemanApplication::settings()->value("General/Language","Persian").toString() )
            setLanguage( lang );
    }
    setLanguage( "Persian" );
}

JamiatApp::~JamiatApp()
{
    if( p->viewer )
        delete p->viewer;

    delete p;
}
