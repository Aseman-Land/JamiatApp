import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    width: parent.width
    color: "#eeeeee"

    Rectangle {
        id: header
        width: parent.width
        height: View.statusBarHeight + Devices.standardTitleBarHeight
        color: "#3192BF"
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: View.statusBarHeight
        anchors.right: parent.right
        titleFont.pixelSize: 13*Devices.fontDensity
        text: qsTr("About Jamiat Imam Ali")
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
                    width: parent.width*0.8
                    height: width*0.83
                    sourceSize: Qt.size(width,height)
                    source: "files/cover2.jpg"
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
                        text: qsTr("About Jamiat Imam Ali")
                    }
                }

                Item {width: 2; height: 20*Devices.density}

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 9*Devices.fontDensity
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                    text: qsTr("About Jamiat Description")

                    Component.onCompleted: {
                        width = flickable.width - 40*Devices.density
                    }
                }
            }
        }
    }

    ScrollBar {
        scrollArea: flickable; height: flickable.height; width: 6*Devices.density
        anchors.right: flickable.right; anchors.top: flickable.top; color: "#3192BF"
    }

    Button {
        id: home_btn
        anchors.bottom: parent.bottom
        anchors.bottomMargin: View.navigationBarHeight + 10*Devices.density
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40*Devices.density
        width: 120*Devices.density
        normalColor: "#3192BF"
        highlightColor: Qt.darker(normalColor)
        textColor: "#ffffff"
        radius: 4*Devices.density
        text: qsTr("Home Page")
        onClicked: Qt.openUrlExternally("http://sosapoverty.org/")
    }
}
