import QtQuick 2.0
import AsemanTools 1.0

Item {
    id: jamiat_images
    width: 100
    height: width*0.75 + thumbnails_list.height
    visible: images.length != 0

    property variant images: new Array
    property color color
    property bool viewMode: false

    onImagesChanged: {
        listv.model.clear()
        if(images.length == 0)
            return;

        listv.model.append({"imageName": images[0]})
        listv.positionViewAtBeginning()
        refresh_more_timer.restart()
    }

    Timer {
        id: refresh_more_timer
        interval: 400
        onTriggered: {
            if(!viewMode)
                return

            for(var i=0; i<images.length; i++)
                thumbnails_list.model.append({"imageName": images[i]})
            for(var i=1; i<images.length; i++)
                listv.model.append({"imageName": images[i]})

            listv.positionViewAtBeginning()
        }
    }

    ListView {
        id: listv
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: thumbnails_list.top
        orientation: Qt.Horizontal
        model: ListModel {}
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange

        delegate: JamiatImage {
            width: listv.width
            height: listv.height
            sourceSize: Qt.size(width, height )
            asynchronous: true
            imageName: model.imageName
            colorAnalizer: index == 0
            onColorChanged: if(index == 0) jamiat_images.color = color
        }
    }

    ListView {
        id: thumbnails_list
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: visible? 48*Devices.density : 0
        orientation: Qt.Horizontal
        model: ListModel {}
        snapMode: ListView.SnapOneItem
        visible: viewMode && count>1
        currentIndex: listv.currentIndex
        highlightMoveDuration: 200
        highlightMoveVelocity: -1

        header: Item {
            height: thumbnails_list.height
            width: {
                var result = (thumbnails_list.width-thumbnails_list.count*height)/2
                if(result<0)
                    return 0
                else
                    return result
            }
        }

        highlight: Rectangle {
            height: thumbnails_list.height
            width: height
            color: jamiat_images.color
            radius: 3*Devices.density
        }

        delegate: Item {
            id: item
            height: thumbnails_list.height
            width: height

            property string imageName: model.imageName

            Image {
                anchors.fill: parent
                anchors.margins: 3*Devices.density
                fillMode: Image.PreserveAspectCrop
                sourceSize: Qt.size(2*width, 2*height )
                asynchronous: true
                source: downloader_item.result
                clip: true

                FileDownloaderQueueItem {
                    id: downloader_item
                    downloaderQueue: downloaderQueueCore
                    fileName: imageName
                    source: "http://aseman.land/jamiat/images/" + item.imageName
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: listv.positionViewAtIndex(index, ListView.Center)
            }
        }
    }
}

