import QtQuick 2.0
import AsemanTools 1.0
import JamiatApp 1.0
import QtGraphicalEffects 1.0

Rectangle {
    id: mview
    color: "#eeeeee"

    property color titleBarColor: search? search_frame.color : post_titlebar.color
    property alias titleBarHeight: cflw.titleBarHeight
    property alias search: search_frame.active

    property bool viewMode: false

    signal searchRequest()
    signal error(string text)

    onViewModeChanged: {
        materialDesignButton.show()
        if( viewMode )
            BackHandler.pushHandler(mview,mview.back)
        else
            BackHandler.removeHandler(mview)
    }

    QtObject {
        id: privates
        property variant item
        property variant loadItem
    }

    MainPageModel{
        id: main_model
        keyword: search_line.text
        onError: mview.error(text)
        Component.onCompleted: readHistory()
    }

    Rectangle {
        id: post_titlebar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: cflw.titleBarHeight
        color: viewMode? privates.item.bodyColor : cflw.titleBarColor

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height/2-height/2 + 2*Devices.density
            font: cflw.titleBarFont
            color: cflw.titleBarDefaultTextColor
            text: privates.item? privates.item.title : ""
        }
    }

    ColorfullListView {
        id: cflw
        width: parent.width
        height: parent.height
        x: viewMode? (View.layoutDirection==Qt.RightToLeft? width : -width) : 0
        listScale: 1 - (refreshing_item.baseNumber)/10
        listTransformOrigin: Item.Bottom

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCirc; duration: 300 }
        }

        bottomMargin: View.navigationBarHeight
        topMargin: 10*Devices.density
        titleBarHeight: 50*Devices.density
        headersHeight: 50*Devices.density
        headerColorHeight: headersHeight
        titleBarFont.pixelSize: 13*Devices.fontDensity
        titleBarFont.family: "B Yekan"
        titleBarDefaultColor: "#689f38"
        model: main_model

        onAtEndChanged: if(atEnd) main_model.loadMore()
        onVerticalVelocityChanged: {
            if((verticalVelocity>4 && !atBegin) || atEnd)
                materialDesignButton.hide()
            else
            if((verticalVelocity<-4 && !atEnd) || atBegin)
                materialDesignButton.show()
        }

        footer: Item {
            width: cflw.width
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
            width: cflw.width

            property variant modelItem

            PostItemBase {
                id: pitem_base
                width: refresh_post_timer.running? parent.width-20 : parent.width
                images: modelItem.images
                postTitle: modelItem.postTitle
                postBody: modelItem.body.slice(0,limit) + "..."
                postDate: modelItem.dateText

                property int limit: images.length==0? 2000 : 500
            }

            Timer {
                id: refresh_post_timer
                interval: 100
                Component.onCompleted: start()
            }

            MouseArea {
                anchors.fill: parent
                anchors.topMargin: pitem_base.titleHeight + pitem_base.margins
                onClicked: {
                    var objY = cflw.mapFromItem(item,0,0).y - postview_frame.y
                    privates.item = post_component.createObject(postview_frame, {"y":objY, "height": item.height, "item": modelItem})
                    viewMode = !viewMode
                }
            }
        }

        Component.onCompleted: {
            model.refresh()
        }
    }

    Rectangle {
        id: search_frame
        anchors.fill: post_titlebar
        color: "#333333"
        visible: active && !viewMode

        property bool active

        onActiveChanged: {
            if(active) {
                BackHandler.pushHandler(search_frame, search_frame.hide)
                search_line.lineFocus = true
            } else {
                BackHandler.removeHandler(search_frame)
                mainFrame.focus = true
            }
        }

        LineEdit {
            id: search_line
            anchors.fill: parent
            anchors.margins: 8*Devices.density
            anchors.rightMargin: 54*Devices.density
            radius: 3*Devices.density
            color: "#555555"
            textColor: "#ffffff"
        }

        function hide() {
            active = false
            search_line.text = ""
        }
    }

    Item {
        id: postview_frame
        anchors.fill: parent
        anchors.topMargin: cflw.titleBarHeight
        clip: true
    }

    JamiatNewRefreshing {
        id: refreshing_item
        anchors.fill: parent
        anchors.topMargin: titleBarHeight
        contentY: cflw.contentY - cflw.originY
        onRefreshRequest: refresh_timer.restart()

        property bool refreshing: main_model.refreshing
        onRefreshingChanged: if(!refreshing) refreshing_item.finish()

        Timer {
            id: refresh_timer
            interval: 2000
            repeat: false
            onTriggered: main_model.refresh()
        }
    }

    MaterialDesignButton {
        id: materialDesignButton
        anchors.fill: parent
        buttonColor: viewMode? privates.item.bodyColor : titleBarColor
        buttonIcon: viewMode? "files/share-light.png" : "files/search.png"
        onClicked: {
            if(viewMode) {
                startIndicator(1000)
                Devices.share(privates.item.postTitle, privates.item.postBody)
            }
            else
                mview.searchRequest()
        }
    }

    Component {
        id: post_component
        PostItem {
            id: pitem
            width: cflw.width
            color: "#eeeeee"
            images: item.images
            postTitle: item.postTitle
            title: item.title
            postBody: item.body
            shareButton: !viewMode
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

    function back() {
        viewMode = false
        privates.item.close()
    }
}
