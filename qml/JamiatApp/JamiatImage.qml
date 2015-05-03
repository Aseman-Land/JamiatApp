import QtQuick 2.0
import AsemanTools 1.0

Image {
    width: 100
    height: width*imageSize.height/imageSize.width
    visible: imageName.length != 0

    property bool colorAnalizer: false
    property string imageName
    property size imageSize: Jamiat.imageSize(Jamiat.fixPath(source))
    property alias color: colorizor.color

    ImageColorAnalizor {
        id: colorizor
        source: colorAnalizer? Jamiat.fixPath(downloader_item.result) : ""
        method: ImageColorAnalizor.MoreSaturation
    }

    FileDownloaderQueueItem {
        id: downloader_item
        downloaderQueue: downloaderQueueCore
        fileName: imageName
        source: "http://aseman.land/jamiat/images/" + imageName
    }

    Indicator {
        anchors.fill: parent
        light: false
        modern: true
        indicatorSize: 22*Devices.density

        property bool active: downloader_item.result.length == 0
        onActiveChanged: {
            if(active)
                start()
            else
                stop()
        }
    }

    Image {
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width, height )
        asynchronous: true
        source: downloader_item.result
    }

    ProgressBar {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 4*Devices.density
        color: "#00000000"
        topColor: "#0d80ec"
        visible: percent != 100
        percent: downloader_item.percent
        radius: 0
    }

    MouseArea {
        anchors.fill: parent
        onClicked: Devices.openFile(downloader_item.result)
    }
}

