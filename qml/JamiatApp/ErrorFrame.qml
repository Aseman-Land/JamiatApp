import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    id: error_frame
    y: View.statusBarHeight + Devices.standardTitleBarHeight
    width: parent.width
    height: error_timer.running? error_text.height + 20*Devices.density : 0
    clip: true
    visible: height != 0

    Behavior on height {
        NumberAnimation{ easing.type: Easing.OutCubic; duration: 200 }
    }

    Text {
        id: error_text
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: AsemanApp.globalFont.family
        font.pixelSize: 11*Devices.density
        color: "#555555"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    Rectangle {
        height: 1*Devices.density
        color: "#eeeeee"
        width: parent.width
        anchors.bottom: parent.bottom
    }

    Timer {
        id: error_timer
        interval: 4000
    }

    function show(text) {
        error_text.text = text
        error_timer.restart()
    }
}

