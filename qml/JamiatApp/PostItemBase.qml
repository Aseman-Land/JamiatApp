import QtQuick 2.0
import AsemanTools 1.0

Item {
    id: pi_base
    width: 100
    height: logicalHeight>maximumHeight? maximumHeight : logicalHeight

    property real margins: Devices.isMobile? 8*Devices.density : 20*Devices.density
    property variant images
    property alias postTitle: pitem_titlebar.title
    property string postBody
    property alias bodyColor: img.color
    property alias postDate: pitem_titlebar.date
    property alias viewMode: img.viewMode

    property real maximumHeight: column.height-body_txt.height + maximumTextHeight
    property real maximumTextHeight: images.length==0? 500*Devices.density : 200*Devices.density
    property real logicalHeight: column.height + 2*margins

    property alias fadeDown: fade_rect.visible
    property alias fadeHeight: fade_rect.height

    property alias titleHeight: pitem_titlebar.height
    property alias shareButton: pitem_titlebar.shareButton

    Rectangle {
        anchors.fill: back
        anchors.margins: -1*Devices.density
        color: "#dddddd"
    }

    Rectangle {
        id: back
        anchors.fill: parent
        anchors.margins: pi_base.margins
        clip: true

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            spacing: 4*Devices.density

            Item { width: 20; height: column.spacing }

            PostItemTitleBar {
                id: pitem_titlebar
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 12*Devices.density
                onShare: Devices.share(postTitle, postBody)
            }

            JamiatImageList {
                id: img
                anchors.left: parent.left
                anchors.right: parent.right
                images: pi_base.images
            }

            Item { width: 20; height: 8*Devices.density }

            Text {
                id: body_txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 12*Devices.density
                font.family: AsemanApp.globalFont.family
                font.pixelSize: Devices.isDesktop? 9*Devices.fontDensity : 8*Devices.fontDensity
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: "#333333"
                textFormat: Text.StyledText
                text: Jamiat.textToHtml(postBody)
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }

        Rectangle {
            id: fade_rect
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 50*Devices.density

            gradient: Gradient {
                GradientStop { position: 0.0; color: "#00000000" }
                GradientStop { position: 1.0; color: "#ffffffff" }
            }
        }
    }
}
