import QtQuick 2.0
import AsemanTools 1.0
import JamiatApp 1.0

Rectangle {
    id: events
    width: 100
    height: 62
    color: "#000000"

    property variant viewItem

    onViewItemChanged: {
        if(viewItem)
            BackHandler.pushHandler(events, events.back)
        else
            BackHandler.removeHandler(events)
    }

    Rectangle {
        id: title_bar
        width: parent.width
        height: Devices.standardTitleBarHeight + View.statusBarHeight
        color: "#FA9401"

        Item {
            anchors.fill: parent
            anchors.topMargin: View.statusBarHeight

            Text {
                anchors.centerIn: parent
                font.pixelSize: 13*Devices.fontDensity
                font.family: "B Yekan"
                color: "#ffffff"
                text: qsTr("Events")
            }
        }
    }

    MainPageModel{
        id: main_model
        onError: error_frame.show(text)
        Component.onCompleted: showEvents()
    }

    Rectangle {
        id: events_scene
        width: parent.width
        anchors.top: title_bar.bottom
        anchors.bottom: parent.bottom
        color: "#eeeeee"
        scale: viewItem? 0.7 : 1
        transformOrigin: Item.Bottom

        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        ListView {
            id: listv
            anchors.fill: parent
            model: main_model
            clip: true

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
                width: listv.width
                height: 92*Devices.density

                property string imageName: {
                    var imgs = modelItem.images
                    if(imgs.length == 0)
                        return ""
                    else
                        return imgs[0]
                }

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 8*Devices.density
                    anchors.leftMargin: 8*Devices.density
                    anchors.rightMargin: 8*Devices.density
                    color: marea.pressed? "#FFE8DA" : "#fcfcfc"
                    border.width: 1*Devices.density
                    border.color: "#dddddd"

                    Image {
                        id: img
                        height: parent.height - 20*Devices.density
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        x: parent.width - width - y
                        fillMode: Image.PreserveAspectCrop
                        sourceSize: Qt.size(2*width, 2*height )
                        asynchronous: true
                        source: downloader_item.result
                        clip: true

                        FileDownloaderQueueItem {
                            id: downloader_item
                            downloaderQueue: downloaderQueueCore
                            fileName: item.imageName
                            source: "http://aseman.land/jamiat/images/" + item.imageName
                        }
                    }

                    Text {
                        width: parent.width - img.width - img.y*3
                        anchors.verticalCenter: parent.verticalCenter
                        x: img.y
                        font.family: AsemanApp.globalFont.family
                        font.pixelSize: 11*Devices.fontDensity
                        color: "#333333"
                        text: modelItem.postTitle
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        elide: Text.ElideRight
                        maximumLineCount: 1
                    }
                }

                MouseArea {
                    id: marea
                    anchors.fill: parent
                    onClicked: {
                        viewItem = event_component.createObject(view_scene, {"eventId": modelItem.eventId})
                    }
                }
            }
        }

        ScrollBar {
            scrollArea: listv; height: listv.height; width: 6*Devices.density
            anchors.right: listv.right; anchors.top: listv.top; color: title_bar.color
        }
    }

    Rectangle {
        id: view_scene
        color: "#eeeeee"
        width: parent.width
        height: events_scene.height
        y: viewItem? events_scene.y : parent.height

        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }
    }

    ErrorFrame {
        id: error_frame
    }

    Component {
        id: event_component
        EventView {
            anchors.fill: parent
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

