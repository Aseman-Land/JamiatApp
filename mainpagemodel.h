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

#ifndef MAINPAGEMODEL_H
#define MAINPAGEMODEL_H

#include "asemantools/asemanabstractcolorfulllistmodel.h"
#include <QColor>

class ApiLayer0_ItemStruct;
typedef ApiLayer0_ItemStruct ApiLayerItemStruct;
class MainPageModel : public AsemanAbstractColorfullListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(QString eventId READ eventId WRITE setEventId NOTIFY eventIdChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)

public:
    enum MainPageModelRoles {
        BodyRole = EndColorfullListModelRoles,
        ImageRole,
        PostTitleRole
    };

    enum RefreshMode {
        NormalState,
        SearchState
    };

    MainPageModel(QObject *parent = 0);
    ~MainPageModel();

    int id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    void setKeyword(const QString &kw);
    QString keyword() const;

    void setEventId(const QString &eventId);
    QString eventId() const;

    int count() const;
    bool refreshing() const;

public slots:
    AsemanColorfullListItem *get( int row );

    void refresh();
    void showEvents();

    void loadMore();
    void readHistory();

signals:
    void countChanged();
    void refreshingChanged();
    void keywordChanged();
    void eventIdChanged();
    void error(const QString &text);

private slots:
    void updateRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void searchRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void lastEventsRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);
    void fetchEventsRequestAnswer(qint64 id, const QList<ApiLayer0_ItemStruct> &items);

    void clearGarbages();
    void error_prv(const QString &text);

private:
    void refresh_prv(QList<ApiLayerItemStruct> items);
    QList<ApiLayer0_ItemStruct> makeUnique(QList<ApiLayer0_ItemStruct> items);
    void insertGarbage(QObject *obj);

private:
    class MainPageModelPrivate *p;
};

#endif // MAINPAGEMODEL_H
