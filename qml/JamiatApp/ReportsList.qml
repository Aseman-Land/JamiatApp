import QtQuick 2.0
import AsemanTools 1.0
import JamiatApp 1.0

Rectangle {
    id: reports
    width: 100
    height: 62
    color: "#eeeeee"

    property variant viewItem

    onViewItemChanged: {
        if(viewItem)
            BackHandler.pushHandler(reports, reports.back)
        else
            BackHandler.removeHandler(reports)
    }

    Rectangle {
        id: title_bar
        width: parent.width
        height: Devices.standardTitleBarHeight + View.statusBarHeight
        color: "#FFA36D"

        Item {
            anchors.fill: parent
            anchors.topMargin: View.statusBarHeight

            Text {
                anchors.centerIn: parent
                font.pixelSize: 13*Devices.fontDensity
                font.family: "B Yekan"
                color: "#ffffff"
                text: qsTr("Reports")
            }
        }
    }

    QtObject {
        id: privates
        property variant item
    }

    MainPageModel{
        id: main_model
        reportMode: true
        Component.onCompleted: {
            readReports()
            refresh()
        }
    }

    ListView {
        id: listv
        width: parent.width
        anchors.top: title_bar.bottom
        anchors.bottom: parent.bottom
        x: postview_frame.viewMode? (View.layoutDirection==Qt.RightToLeft? width : -width) : 0
        model: main_model
        clip: true
        onAtYEndChanged: if(atYEnd) main_model.loadMore()

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCirc; duration: 300 }
        }

        footer: Item {
            width: listv.width
            height: 120*Devices.density + View.navigationBarHeight

            Indicator {
                width: parent.width
                height: parent.height - View.navigationBarHeight
                light: false
                modern: true
                indicatorSize: 18*Devices.density

                property bool active: main_model.refreshing
                onActiveChanged: active? start() : stop()
            }
        }

        delegate: Item {
            id: item
            height: pitem_base.height
            width: listv.width

            PostItemBase {
                id: pitem_base
                width: parent.width
                images: modelItem.images
                postTitle: modelItem.postTitle
                postBody: modelItem.body.slice(0,limit) + "..."
                postDate: modelItem.dateText

                property int limit: images.length==0? 2000 : 500
            }

            MouseArea {
                anchors.fill: parent
                anchors.topMargin: pitem_base.titleHeight + pitem_base.margins
                onClicked: {
                    var objY = listv.mapFromItem(item,0,0).y
                    privates.item = post_component.createObject(postview_frame, {"y":objY, "height": item.height, "item": modelItem})
                    postview_frame.viewMode = !postview_frame.viewMode
                }
            }
        }
    }

    ScrollBar {
        scrollArea: listv; height: listv.height; width: 6*Devices.density
        anchors.right: listv.right; anchors.top: listv.top; color: "#FA9401"
    }

    Item {
        id: postview_frame
        width: parent.width
        anchors.top: title_bar.bottom
        anchors.bottom: parent.bottom
        clip: true

        property bool viewMode

        onViewModeChanged: {
            if( viewMode ) {
                BackHandler.pushHandler(postview_frame,postview_frame.back)
                materialDesignButton.show()
            } else {
                BackHandler.removeHandler(postview_frame)
                materialDesignButton.hide()
            }
        }

        function back() {
            viewMode = false
            privates.item.close()
        }
    }

    MaterialDesignButton {
        id: materialDesignButton
        anchors.fill: parent
        buttonColor: "#FA9401"
        buttonIcon: "files/share-light.png"
        onClicked: {
            startIndicator(1000)
            Devices.share(privates.item.postTitle, privates.item.postBody)
        }
        Component.onCompleted: hide()
    }

    Component {
        id: post_component
        PostItem {
            id: pitem
            width: listv.width
            color: "#eeeeee"
            images: item.images
            postTitle: item.postTitle
            title: item.title
            postBody: item.body
            shareButton: !postview_frame.viewMode
            titleColor: item.color
            postDate: item.dateText
            link: item.link

            onVerticalVelocityChanged: {
                if((verticalVelocity>4 && !atYBeginning) || atYEnd)
                    materialDesignButton.hide()
                else
                if((verticalVelocity<-4 && !atYEnd) || atYBeginning)
                    materialDesignButton.show()
            }

            property variant item
        }
    }

    Timer {
        id: destroyer_timer
        interval: 400
        onTriggered: item.destroy()
        property variant item
    }

    function back() {
        if(!viewItem)
            return

        destroyer_timer.item = viewItem
        destroyer_timer.restart()
        viewItem = 0
    }
}

