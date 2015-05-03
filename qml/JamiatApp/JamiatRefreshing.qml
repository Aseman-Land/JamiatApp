import QtQuick 2.2
import AsemanTools 1.0

Item {
    id: jrfr
    anchors.fill: parent

    property real contentY
    property real baseNumber: contentY>0? 0 : -contentY/(height*0.4)
    property real circlesWidth: 18*Devices.density
    property real circlesMargin: 50*Devices.density

    signal refreshRequest()

    Behavior on opacity {
        NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0,0,0, privates.dynamicBase>0.6? 0.6 : privates.dynamicBase)
    }

    MouseArea {
        anchors.fill: parent
        visible: privates.refreshing
    }

    QtObject {
        id: privates

        property bool signalBlocker: false
        property bool widthAnimations: false
        property bool rotateAnimations: false
        property bool refreshing: false
        property real dynamicBase: privates.refreshing? 0.4 : baseNumber
        property bool refreshingFlag: false
        property real refreshWidth: baseNumber>0? Math.max(jrfr.width, jrfr.height)/(baseNumber*20) :
                                                  Math.max(jrfr.width, jrfr.height)

        onRefreshWidthChanged:  {
            if(privates.refreshing)
                return
            if(privates.signalBlocker)
                return
            if(refreshWidth < circlesMargin)
                privates.refreshingFlag = true
            else
            if(privates.refreshingFlag && refreshWidth >= circlesMargin) {
                privates.widthAnimations = true
                privates.refreshing = true
                refreshRequest()
            }
        }
    }

    Text {
        id: refresh_text
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter
        anchors.topMargin: 50*Devices.density
        color: "#ffffff"
        font.family: AsemanApp.globalFont.family
        font.pixelSize: 12*Devices.fontDensity
        text: qsTr("Refreshing")
        opacity: 0

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 500 }
        }

        Timer {
            interval: 500
            repeat: true
            triggeredOnStart: true

            property bool stateFlag: false
            property bool refreshing: privates.refreshing

            onRefreshingChanged: {
                if(refreshing) {
                    stateFlag = false
                    restart()
                } else {
                    stateFlag = false
                    stop()
                    refresh_text.opacity = 0
                }
            }
            onTriggered: {
                stateFlag = !stateFlag
                refresh_text.opacity = stateFlag? 1 : 0.5
            }
        }
    }

    Component {
        id: circle_component
        Item {
            id: circle_frame
            anchors.centerIn: parent
            width: privates.refreshing? circlesMargin : privates.refreshWidth
            height: width
            transformOrigin: Item.Center
            rotation: 45 - startRotation + (privates.refreshing? rotatePad : -baseNumber*360)

            property real startRotation: 0
            property real rotatePad: 0
            property color color
            property bool refreshingState: privates.refreshing

            onRefreshingStateChanged: {
                if(refreshingState) {
                    rotatePad = -baseNumber*360
                    privates.rotateAnimations = true
                    rotate_timer.restart()
                } else {
                    rotate_timer.stop()
                }
            }

            Behavior on rotation {
                NumberAnimation{ easing.type: Easing.Linear; duration: privates.rotateAnimations? 500 : 0 }
            }
            Behavior on width {
                NumberAnimation{ easing.type: Easing.OutCubic; duration: privates.widthAnimations? 800 : 0 }
            }

            Timer {
                id: rotate_timer
                triggeredOnStart: true
                interval: 500
                repeat: true
                onTriggered: rotatePad += 180
            }

            Rectangle {
                width: circlesWidth
                height: width
                radius: width/2
                color: circle_frame.color
                border.width: 1*Devices.density
                border.color: "#888888"
            }
        }
    }

    Timer {
        id: anim_disabler_timer
        interval: 400
        onTriggered: {
            privates.signalBlocker = true
            privates.rotateAnimations = false
            privates.widthAnimations = false
            privates.refreshing = false
            privates.refreshingFlag = false
            privates.signalBlocker = false
            jrfr.opacity = 1
        }
    }

    function finish() {
        anim_disabler_timer.restart()
        jrfr.opacity = 0
    }

    Component.onCompleted: {
        circle_component.createObject(jrfr, {"color": "#ffffff", "startRotation": 060})
        circle_component.createObject(jrfr, {"color": "#ffffff", "startRotation": 180})
        circle_component.createObject(jrfr, {"color": "#ffffff", "startRotation": 300})
    }
}
