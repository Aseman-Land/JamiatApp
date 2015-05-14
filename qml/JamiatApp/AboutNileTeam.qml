import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    width: parent.width
    color: "#eeeeee"

    Rectangle {
        id: header
        width: parent.width
        height: View.statusBarHeight + Devices.standardTitleBarHeight
        color: "#00A0E3"
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: View.statusBarHeight
        anchors.right: parent.right
        titleFont.pixelSize: 11*Devices.fontDensity
        titleFont.family: "Droid Arabic Naskh"
        text: qsTr("About Nile Group")
        light: true
        backButton: false
    }

    Flickable {
        id: flickable
        anchors.top: header.bottom
        anchors.bottom: home_btn.top
        width: parent.width
        contentWidth: column.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Item {
            id: main_item
            width: flickable.width
            height: column.height>flickable.height? column.height : flickable.height

            Column {
                id: column
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20*Devices.density
                anchors.verticalCenter: parent.verticalCenter

                Item {width: 20; height: 20*Devices.density}

                Image {
                    width: 150*Devices.density
                    height: width
                    sourceSize: Qt.size(width,height)
                    source: "files/nilegroup.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {width: 2; height: 20*Devices.density}

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: -20*Devices.density
                    height: 50*Devices.density
                    color: "#dddddd"

                    Text {
                        anchors.centerIn: parent
                        font.family: AsemanApp.globalFont.family
                        font.pixelSize: 11*Devices.fontDensity
                        font.bold: true
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        color: "#333333"
                        text: qsTr("About Nile Group")
                    }
                }

                Item {width: 2; height: 20*Devices.density}

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 9*Devices.fontDensity
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                    text: qsTr("Nile is an Iranian software corporation that makes software for Desktop computers, Android, iOS, Mac, Windows Phone, Ubuntu Phone and ...\n"+
                               "Nile create Free and OpenSource projects.")

                    Component.onCompleted: {
                        width = flickable.width - 40*Devices.density
                    }
                }
            }
        }
    }

    ScrollBar {
        scrollArea: flickable; height: flickable.height; width: 6*Devices.density
        anchors.right: flickable.right; anchors.top: flickable.top; color: "#00A0E3"
    }

    Button {
        id: home_btn
        anchors.bottom: parent.bottom
        anchors.bottomMargin: View.navigationBarHeight + 10*Devices.density
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40*Devices.density
        width: 120*Devices.density
        normalColor: "#00A0E3"
        highlightColor: Qt.darker(normalColor)
        textColor: "#ffffff"
        radius: 4*Devices.density
        text: qsTr("Home Page")
        onClicked: Qt.openUrlExternally("http://nilegroup.org")
    }
}
