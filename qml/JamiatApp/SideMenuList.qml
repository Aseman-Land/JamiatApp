import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    id: smlist
    width: 100
    height: 62

    signal itemClicked(string cmd)

    ListView {
        id: list
        anchors.fill: parent
        clip: true
        model: ListModel{}
        header: Item{ width: list.width; height: 10*Devices.density }
        delegate: Item {
            width: list.width
            height: 40*Devices.density

            Rectangle {
                anchors.fill: parent
                opacity: 0.6
                color: marea.pressed? "#0d80ec" : "#00000000"
            }

            Image {
                width: 22*Devices.density
                height: width
                anchors.right: parent.right
                anchors.rightMargin: 10*Devices.density
                sourceSize: Qt.size(width, height)
                source: icon
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                anchors.fill: parent
                anchors.leftMargin: 40*Devices.density
                anchors.rightMargin: 40*Devices.density
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: View.layoutDirection==Qt.LeftToRight? Text.AlignLeft : Text.AlignRight
                font.family: "B Yekan"
                font.pixelSize: 11*Devices.fontDensity
                color: "#333333"
                text: txt
            }

            MouseArea {
                id: marea
                anchors.fill: parent
                onClicked: smlist.itemClicked(cmd)
            }
        }

        function refresh() {
            model.clear()
            model.append( {"txt":qsTr("Home"), "icon": "files/menu-home.png", "cmd": "home"} )
            model.append( {"txt":qsTr("Reports"), "icon": "files/menu-reports.png", "cmd": "reports"} )
            model.append( {"txt":qsTr("Events"), "icon": "files/menu-events.png", "cmd": "events"} )
            model.append( {"txt":qsTr("OpenSource Licenses"), "icon": "files/menu-opensource.png", "cmd": "opensource-apps"} )
            model.append( {"txt":qsTr("About Jamiat"), "icon": "files/menu-about.png", "cmd": "about-jamiat"} )
            model.append( {"txt":qsTr("About Application"), "icon": "files/menu-about.png", "cmd": "about-app"} )
//            model.append( {"txt":qsTr("About NileGroup"), "cmd": "about-nile"} )
        }
    }

    ScrollBar {
        anchors.top: list.top
        x: View.layoutDirection==Qt.LeftToRight? list.width-width : 0
        scrollArea: list
        height: list.height
        width: 6*Devices.density
        color: "#333333"
    }

    Connections{
        target: AsemanApp
        onLanguageUpdated: initTranslations()
    }

    function initTranslations(){
        list.refresh()
    }

    Component.onCompleted: initTranslations()
}
