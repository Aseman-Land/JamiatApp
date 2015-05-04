/*
    Copyright (C) 2015 Nile Group
    http://nilegroup.org

    Meikade is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Meikade is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    id: about
    width: 100
    height: 62
    color: "#eeeeee"

    Rectangle {
        id: header
        width: parent.width
        height: View.statusBarHeight + Devices.standardTitleBarHeight
        color: "#A80807"
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: View.statusBarHeight
        anchors.right: parent.right
        titleFont.pixelSize: 13*Devices.fontDensity
        text: qsTr("About")
        light: true
        backButton: false
    }

    Flickable {
        id: flickable
        anchors.top: header.bottom
        anchors.bottom: home_btn.top
        anchors.bottomMargin: 40*Devices.density
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
                    width: 128*Devices.density
                    height: width
                    sourceSize: Qt.size(width,height)
                    source: "files/jamiat.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {width: 2; height: 20*Devices.density}

                Text {
                    width: parent.width
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 10*Devices.fontDensity
                    font.bold: true
                    text: qsTr("This App is a free and opensource application by Nile Team")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                }

                Text {
                    width: parent.width
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 9*Devices.fontDensity
                    text: qsTr("This App is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                }

                Item {width: 20; height: 30*Devices.density}

                Text {
                    width: parent.width
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 10*Devices.fontDensity
                    font.bold: true
                    text: qsTr("Members:")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                }

                Text {
                    width: parent.width
                    font.family: AsemanApp.globalFont.family
                    font.pixelSize: 9*Devices.fontDensity
                    text: qsTr(" - Bardia Daneshvar (Project Leader, UI Designer and Developer)\n"+
                               " - AmirHosein Mousavi (Art Designer and Developer)")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#333333"
                }
            }
        }
    }

    ScrollBar {
        scrollArea: flickable; height: flickable.height; width: 6*Devices.density
        anchors.right: flickable.right; anchors.top: flickable.top; color: "#A80807"
    }

    Button {
        id: home_btn
        anchors.bottom: parent.bottom
        anchors.bottomMargin: View.navigationBarHeight + 10*Devices.density
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40*Devices.density
        width: 150*Devices.density
        normalColor: "#A80807"
        highlightColor: Qt.darker(normalColor)
        textColor: "#ffffff"
        radius: 4*Devices.density
        text: qsTr("Jamiat App Github")
        onClicked: Qt.openUrlExternally("https://github.com/NileGroup/JamiatApp")
    }

    Text {
        id: version
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 20*Devices.density
        anchors.bottomMargin: View.navigationBarHeight + 10*Devices.density
        font.family: AsemanApp.globalFont.family
        font.pixelSize: 9*Devices.fontDensity
        text: "v0.7.0"
        wrapMode: Text.WordWrap
        color: "#333333"
        horizontalAlignment: Text.AlignRight
    }
}
