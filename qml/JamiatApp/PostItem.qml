import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    id: post_item
    width: 100
    height: pibase.height
    x: 0

    property color titleColor
    property string title
    property alias postTitle: pibase.postTitle
    property alias postBody: pibase.postBody
    property color bodyColor: pibase.bodyColor=="#000000"? titleColor : pibase.bodyColor
    property alias postDate: pibase.postDate
    property string link
    property alias images: pibase.images
    property alias shareButton: pitem_tbar.shareButton

    property alias verticalVelocity: flick.verticalVelocity
    property alias horizontalVelocity: flick.horizontalVelocity
    property alias atYBeginning: flick.atYBeginning
    property alias atYEnd: flick.atYEnd

    QtObject {
        id: privates
        property real startH
        property real startY
    }

    Behavior on height {
        NumberAnimation{ easing.type: Easing.OutCirc; duration: 300 }
    }
    Behavior on y {
        NumberAnimation{ easing.type: Easing.OutCirc; duration: 300 }
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: post_frame.height
        contentWidth: post_frame.width
        bottomMargin: View.navigationBarHeight
        maximumFlickVelocity: View.flickVelocity
        clip: true

        property bool atFirst: atYBeginning

        onAtFirstChanged: {
            if( !atFirst )
                BackHandler.pushHandler(flick,flick.gotoBegin)
            else
                BackHandler.removeHandler(flick)
        }

        NumberAnimation { id: anim; target: flick; easing.type: Easing.OutCubic; property: "contentY"; duration: 300 }

        Item {
            id: post_frame
            width: flick.width
            height: pibase.height + 48*Devices.density

            PostItemBase {
                id: pibase
                width: parent.width
                height: logicalHeight
                fadeDown: false
                shareButton: pitem_tbar.shareButton
                viewMode: true
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10*Devices.density
                width: 100*Devices.density
                height: 36*Devices.density
                normalColor: pibase.bodyColor
                highlightColor: Qt.darker(normalColor)
                textColor: "#ffffff"
                textFont.family: AsemanApp.globalFont.family
                textFont.pixelSize: 9*Devices.fontDensity
                text: qsTr("Go To Resource")
                radius: 3*Devices.density
                onClicked: Qt.openUrlExternally(link)
            }
        }

        function gotoBegin() {
            anim.from = contentY;
            anim.to = 0;
            anim.running = true;
        }
    }

    ScrollBar {
        scrollArea: flick; height: flick.height; width: 6*Devices.density
        anchors.right: flick.right; anchors.top: flick.top; color: bodyColor
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        color: "#eeffffff"
        height: pitem_tbar.height
        visible: Devices.isMobile? flick.contentY>14*Devices.density : flick.contentY>26*Devices.density

        PostItemTitleBar {
            id: pitem_tbar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12*Devices.density + pibase.margins
            title: postTitle
            date: pibase.postDate
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.bottom
            color: "#dddddd"
            height: 1*Devices.density
        }
    }

    Timer {
        id: init_timer
        interval: 300
        onTriggered: {
            privates.startY = y
            privates.startH = height
            y = 0
            height = post_item.parent.height
        }
    }

    Timer {
        id: destroy_timer
        interval: 300
        onTriggered: post_item.destroy()
    }

    function close() {
        y = privates.startY
        height = privates.startH
        destroy_timer.restart()
        fade_rect.visible = true
    }

    Component.onCompleted: {
        init_timer.restart()
    }
}
