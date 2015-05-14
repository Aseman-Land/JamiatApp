import QtQuick 2.0
import AsemanTools 1.0

Row {
    id: title_row
    layoutDirection: View.layoutDirection
    height: Devices.standardTitleBarHeight

    property alias title: txt.text
    property alias date: date_txt.text
    property alias shareButton: share_btn.visible

    signal share()

    Column {
        id: title_column
        width: share_btn.visible? title_row.width - share_btn.width : title_row.width
        anchors.verticalCenter: parent.verticalCenter
        spacing: -4*Devices.density

        Text {
            id: txt
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: 11*Devices.fontDensity
            font.family: "Droid Arabic Naskh"
            color: "#222222"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            maximumLineCount: 1
            elide: Text.ElideRight
            horizontalAlignment: title_row.layoutDirection==Qt.RightToLeft? Text.AlignRight : Text.AlignLeft
        }

        Text {
            id: date_txt
            anchors.left: parent.left
            anchors.right: parent.right
            font.family: "Droid Arabic Naskh"
            font.pixelSize: 8*Devices.fontDensity
            color: "#666666"
            horizontalAlignment: title_row.layoutDirection==Qt.RightToLeft? Text.AlignRight : Text.AlignLeft
        }
    }

    Button {
        id: share_btn
        anchors.verticalCenter: parent.verticalCenter
        normalColor: "#00000000"
        highlightColor: "#11000000"
        border.color: "#dddddd"
        height: 26*Devices.density
        border.width: 1*Devices.density
        iconHeight: 14*Devices.density
        icon: "files/share.png"
        onClicked: title_row.share()
        transform: Scale {
            origin.x: share_btn.width/2
            origin.y: share_btn.height
            xScale: View.layoutDirection==Qt.RightToLeft? -1 : 1
        }
    }
}
