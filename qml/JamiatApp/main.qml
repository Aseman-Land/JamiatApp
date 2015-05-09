import QtQuick 2.2
import AsemanTools 1.0

AsemanMain {
    visible: true
    height: 640
    width: 480
    focus: true
    mainFrame: mview
    color: "#000000"

    property bool menuMode: false
    property alias downloaderQueueCore: downloader_queue

    onMenuModeChanged: {
        if(menuMode)
            BackHandler.pushHandler(menu_frame, menu_frame.back)
        else
            BackHandler.removeHandler(menu_frame)
    }

    FontLoader{
        id: papyrus_normal_font
        source: Devices.resourcePath + "/fonts/DroidKaqazSans.ttf"
        onStatusChanged: if(status == FontLoader.Ready) AsemanApp.globalFont.family = name
    }

    Connections {
        target: Jamiat
        onBackRequest: AsemanApp.back()
    }

    Connections {
        target: AsemanApp
        onBackRequest: {
            var res = BackHandler.back()
            if( !res && !Devices.isDesktop )
                Jamiat.close()
        }
    }

    Keys.onEscapePressed: {
        AsemanApp.back()
    }

    FileDownloaderQueue {
        id: downloader_queue
        capacity: 3
        destination: Devices.picturesLocation + "/Jamiat Imam Ali"
    }

    Item {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        scale: menuMode? 0.8 : 1
        transformOrigin: Item.Bottom
        clip: true

        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        MainView {
            id: mview
            anchors.top: stat_back.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onError: error_frame.show(text)
            onSearchRequest: {
                mview.search = true
            }
        }

        Rectangle {
            id: stat_back
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: View.statusBarHeight
            color: mview.titleBarColor
        }

        ErrorFrame {
            id: error_frame
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: sidebar.top
            anchors.bottom: sidebar.bottom
            color: "#000000"
            opacity: sidebar.percent*0.7
        }

        SideMenu {
            id: sidebar
            anchors.fill: parent
            menuWidth: {
                var result = parent.width/2 + 30*Devices.density
                if(result < 200*Devices.density)
                    result = parent.width-80*Devices.density

                return result
            }

            delegate: MouseArea {
                anchors.fill: parent

                Image {
                    id: menu_cover
                    width: parent.width
                    height: width*9/16
                    source: "files/cover.jpg"
                    sourceSize: Qt.size(width,height)
                }

                SideMenuList {
                    anchors.top: menu_cover.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    color: "#f0f0f0"
                    onItemClicked: {
                        var component
                        switch(cmd)
                        {
                        case "home":
                            menuMode = false
                            sidebar.discard()
                            return

                        case "events":
                            sidebar.discard()
                            component = events_list_component
                            break

                        case "about-nile":
                            component = about_nile_component
                            break;

                        case "about-app":
                            component = about_app_component
                            break;

                        case "about-jamiat":
                            component = about_jamiat_component
                            break;

                        case "opensource-apps":
                            component = opensource_component
                            break;
                        }

                        menu_hider.restart()
                        if(component) {
                            var item = component.createObject(menu_frame)
                            menu_frame.item = item
                        }

                        menuMode = true
                    }

                    Timer {
                        id: menu_hider
                        interval: 400
                        onTriggered: sidebar.discard()
                    }
                }
            }
        }

        Button {
            id: jamiat_btn
            x: View.layoutDirection==Qt.RightToLeft? parent.width-width : 0
            anchors.top: mview.top
            height: mview.titleBarHeight - anchors.margins*2
            icon: "files/icon.png"
            normalColor: "#00000000"
            highlightColor: "#44ffffff"
            iconHeight: height*0.5
            width: height*1.3
            opacity: 1-sidebar.percent
            onClicked: sidebar.show()

            Image {
                x: View.layoutDirection==Qt.RightToLeft? parent.width-width/2 : -width/2
                height: parent.height*0.4
                width: height
                anchors.verticalCenter: parent.verticalCenter
                sourceSize: Qt.size(width,height)
                source: "files/menu.png"
            }
        }
    }

    Rectangle {
        id: menu_frame
        color: "#ededed"
        y: menuMode? 0 : height
        width: parent.width
        height: parent.height

        property variant item

        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        function back() {
            menuMode = false
            item_destroyer.restart()
        }

        Timer {
            id: item_destroyer
            interval: 400
            onTriggered: if(menu_frame.item) menu_frame.item.destroy()
        }
    }

    Rectangle {
        width: parent.width
        height: View.navigationBarHeight
        anchors.bottom: parent.bottom
        color: "#000000"
    }

    Component {
        id: events_list_component
        EventsList {
            anchors.fill: parent
        }
    }

    Component {
        id: about_nile_component
        AboutNileTeam {
            anchors.fill: parent
        }
    }

    Component {
        id: about_app_component
        About {
            anchors.fill: parent
        }
    }

    Component {
        id: about_jamiat_component
        AboutJamiat {
            anchors.fill: parent
        }
    }

    Component {
        id: opensource_component
        OpenSourceProjects {
            anchors.fill: parent
        }
    }

    Component.onCompleted: {
        View.layoutDirection = Qt.RightToLeft
    }
}
