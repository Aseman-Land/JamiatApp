import QtQuick 2.0
import AsemanTools 1.0

Item {
    width: 100
    height: 62
    clip: true

    property real contentY
    property real baseNumber: contentY>0? 0 : -contentY/(height*0.4)
    property real headHeight: 50*Devices.density
    property bool crashed: false

    signal refreshRequest()

    Rectangle {
        id: header
        width: parent.width
        color: "#eeffffff"
        clip: true
        y: crashed? parent.height - headHeight - View.navigationBarHeight : 0
        rotation: crashed? 10 : 0
        height: {
            if(refreshing)
                return headHeight
            if(prepareToRefresh && nwHeight < headHeight)
                return headHeight
            else
            if(nwHeight > headHeight)
                return headHeight
            else
                return nwHeight
        }
        border.width: 1*Devices.density
        border.color: "#dddddd"

        property bool prepareToRefresh
        property bool refreshing: false
        property real nwHeight: baseNumber*150*Devices.density
        property bool heightAnim: false

        Behavior on height {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: header.heightAnim? 400 : 0 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutBounce; duration: 600 }
        }
        Behavior on rotation {
            NumberAnimation{ easing.type: Easing.OutElastic; easing.amplitude: 2.0 ; duration: 3000 }
        }

        transform: Scale {
            origin.x: width/2
            origin.y: 0
            yScale: {
                if( crashed )
                    return 1
                if( header.nwHeight>=headHeight )
                    return header.nwHeight/headHeight
                else
                    return 1
            }
            onYScaleChanged: {
                if(crashed)
                    return

                if(yScale > 1)
                    header.prepareToRefresh = true
                else {
                    if(header.prepareToRefresh)
                        header.refreshing = true

                    header.prepareToRefresh = false
                }
            }

            property bool prepareToRefresh

            Behavior on yScale {
                NumberAnimation { easing.type: Easing.OutElastic; easing.amplitude: 0.6 ; duration: 1000 }
            }
        }

        Item {
            width: parent.width
            height: headHeight
            anchors.bottom: parent.bottom

            Text {
                anchors.centerIn: parent
                color: "#666666"
                font.family: AsemanApp.globalFont.family
                font.pixelSize: 10*Devices.fontDensity
                visible: !indicator.visible
                text: {
                    if(crashed)
                        return qsTr(":|")
                    if(!header.prepareToRefresh)
                        return qsTr("Drag to Refresh")
                    else
                    if(header.nwHeight > headHeight*2.2)
                        return qsTr("Drop Me :((")
                    else
    //                if(header.nwHeight > headHeight*2.5)
    //                    return qsTr(":((")
    //                else
                    if(header.nwHeight > headHeight*1.8)
                        return qsTr("Outch!! :(")
                    else
                        return qsTr("Now drop :)")
                }
            }

            Indicator {
                id: indicator
                anchors.centerIn: parent
                light: false
                modern: true
                indicatorSize: 22*Devices.density
                visible: !crashed && header.refreshing && header.nwHeight<=headHeight
            }
        }

        Timer {
            id: height_anim_timer
            interval: 400
            onTriggered: header.heightAnim = false
        }

        onNwHeightChanged: {
            if(nwHeight > headHeight*2.6)
                crashed = true
        }

        onRefreshingChanged: {
            if(refreshing) {
                indicator.start()
                refreshRequest()
            } else {
                indicator.stop()
            }
        }
    }

    function finish() {
        header.heightAnim = true
        header.refreshing = false
        height_anim_timer.restart()
    }
}

