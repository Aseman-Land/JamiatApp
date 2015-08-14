import QtQuick 2.2
import AsemanTools 1.0

AsemanMain {
    visible: true
    height: 640
    width: 480
    focus: true
    mainFrame: stack_view
    color: "#000000"

    property alias downloaderQueueCore: downloader_queue

    FontLoader{
        id: papyrus_normal_font
        source: Devices.resourcePath + "/fonts/DroidKaqazSans.ttf"
        onStatusChanged: if(status == FontLoader.Ready) AsemanApp.globalFont.family = name
    }

    FontLoader{
        id: papyrus_yekan_font
        source: Devices.resourcePath + "/fonts/BYekan.ttf"
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

    StackViewer {
        id: stack_view
        anchors.fill: parent
        mainDelegate: Item {
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            transformOrigin: Item.Bottom
            clip: true

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
        }
    }

    Item {
        anchors.fill: parent

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
            anchors.bottomMargin: View.navigationBarHeight
            menuWidth: Devices.isMobile? parent.width-50*Devices.density : parent.width/2 + 50*Devices.density

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
                    anchors.bottom: menu_cover_btm.top
                    width: parent.width
                    color: "#f0f0f0"
                    onItemClicked: {
                        var component
                        switch(cmd)
                        {
                        case "home":
                            stack_view.home()
                            sidebar.discard()
                            return

                        case "events":
                            component = events_list_component
                            break

                        case "reports":
                            component = reports_list_component
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

                        sidebar.discard()
                        if(component)
                            stack_view.append(component)
                    }
                }

                Image {
                    id: menu_cover_btm
                    width: parent.width
                    height: width/6
                    anchors.bottom: parent.bottom
                    source: "files/cover3.jpg"
                    sourceSize: Qt.size(width,height)
                }
            }
        }

        Button {
            id: jamiat_btn
            x: View.layoutDirection==Qt.RightToLeft? parent.width-width : 0
            anchors.top: parent.top
            anchors.topMargin: View.statusBarHeight
            height: Devices.standardTitleBarHeight - anchors.margins*2
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
        id: reports_list_component
        ReportsList {
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
