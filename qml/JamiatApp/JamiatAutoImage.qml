import QtQuick 2.0

Item {
    id: auto_image
    width: 100
    height: width*0.75 + (images.length>1 && viewMode? thumbnailBarHeight : 0)
    visible: images.length != 0

    property variant images: new Array
    property color color
    property bool viewMode: false
    property real thumbnailBarHeight: 48*Devices.density

    onImagesChanged: {
        if(privates.item)
            privates.item.destroy()

        if(images.length == 0)
            return
        else
        if(images.length == 1)
            privates.item = self_component.createObject(auto_image)
        else
            privates.item = list_component.createObject(auto_image)
    }

    QtObject {
        id: privates
        property variant item
    }

    Component {
        id: self_component
        JamiatImage {
            anchors.fill: parent
            imageName: auto_image.images[0]
            onColorChanged: auto_image.color = color
        }
    }

    Component {
        id: list_component
        JamiatImageList {
            anchors.fill: parent
            images: auto_image.images
            viewMode: auto_image.viewMode
            onColorChanged: auto_image.color = color
            thumbnailBarHeight: auto_image.thumbnailBarHeight
        }
    }
}

