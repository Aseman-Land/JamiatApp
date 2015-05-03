import QtQuick 2.0
import AsemanTools 1.0
import QtGraphicalEffects 1.0

Item {
    id: md_btn
    anchors.fill: parent

    property alias buttonColor: button_rect.color
    property string buttonIcon
    property bool hideState: false

    signal clicked()

    DropShadow {
        anchors.fill: source
        horizontalOffset: 0
        verticalOffset: 1*Devices.density
        radius: 8.0
        samples: 16
        color: "#80000000"
        source: mask_item
    }

    Timer {
        id: init_timer
        interval: 1000
        onTriggered: inited = true
        Component.onCompleted: start()

        property bool inited: false
    }

    Item {
        id: mask_item
        x: parent.width - width
        y: hideState? parent.height+20*Devices.density : parent.height - height - View.navigationBarHeight
        width: 64*Devices.density
        height: 64*Devices.density

        Behavior on y {
            NumberAnimation{easing.type: Easing.OutCubic; duration: init_timer.inited? 400 : 0}
        }

        Rectangle {
            id: button_rect
            anchors.fill: parent
            anchors.margins: 8*Devices.density
            radius: width/2

            Image {
                id: button_img
                anchors.fill: parent
                anchors.margins: 9*Devices.density
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(width, height)
                source: buttonIcon
                transform: Scale {
                    origin.x: button_img.width/2
                    origin.y: button_img.height
                    xScale: View.layoutDirection==Qt.RightToLeft? -1 : 1
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: md_btn.clicked()
        }
    }

    function hide() {
        hideState = true
    }

    function show() {
        hideState = false
    }
}

