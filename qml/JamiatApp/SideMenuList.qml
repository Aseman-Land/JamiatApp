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
                color: marea.pressed? masterPalette.highlight : "#00000000"
            }

            Text {
                anchors.fill: parent
                anchors.leftMargin: 20*Devices.density
                anchors.rightMargin: 20*Devices.density
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: View.layoutDirection==Qt.LeftToRight? Text.AlignLeft : Text.AlignRight
                font.family: AsemanApp.globalFont.family
                font.pixelSize: 9*Devices.fontDensity
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
            model.append( {"txt":qsTr("Home"), "cmd": "home"} )
            model.append( {"txt":qsTr("Events"), "cmd": "events"} )
            model.append( {"txt":qsTr("OpenSource Licenses"), "cmd": "opensource-apps"} )
            model.append( {"txt":qsTr("About Jamiat"), "cmd": "about-jamiat"} )
            model.append( {"txt":qsTr("About Application"), "cmd": "about-app"} )
            model.append( {"txt":qsTr("About NileGroup"), "cmd": "about-nile"} )
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
