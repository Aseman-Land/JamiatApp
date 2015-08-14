import QtQuick 2.0
import AsemanTools 1.0

Rectangle {
    id: stack_viewer
    width: 100
    height: 62
    color: "#000000"

    property color frameColor: "#eeeeee"
    property Component mainDelegate

    onMainDelegateChanged: {
        if(privates.item)
            privates.item.destroy()
        if(!mainDelegate)
            return

        privates.item = mainDelegate.createObject(main_scene)
    }

    QtObject {
        id: privates
        property variant item
    }

    ListObject {
        id: list
    }

    Rectangle {
        id: main_scene
        width: parent.width
        height: parent.height
        color: frameColor
        transformOrigin: Item.Bottom
        scale: list.count==0? 1 : 0.8

        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }
    }

    Component {
        id: single_scene_component
        Rectangle {
            id: single_scene_item
            width: parent.width
            height: parent.height
            color: frameColor
            transformOrigin: Item.Bottom
            scale: opened? 1 : 0.8
            y: parent.height

            property bool anims: false
            property int index: 0
            property bool opened: list.count==index+1
            property Component cmpnt
            property variant item

            Behavior on scale {
                NumberAnimation{ easing.type: Easing.OutCubic; duration: single_scene_item.anims? 400 : 0 }
            }
            Behavior on y {
                NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
            }

            Timer {
                id: destroyer
                interval: 400
                onTriggered: single_scene_item.destroyItem()
            }

            Component.onCompleted: {
                index = list.count
                item = cmpnt.createObject(single_scene_item)
            }

            Component.onDestruction: {
                BackHandler.removeHandler(single_scene_item)
            }

            function show() {
                y = 0
                anims = true
                BackHandler.pushHandler(single_scene_item, single_scene_item.back)
            }

            function close() {
                scale = 1
                y = stack_viewer.height
                destroyer.restart()
            }

            function destroyItem() {
                View.root.focus = true
                item.destroy()
                destroy()
            }

            function back() {
                stack_viewer.back()
            }
        }
    }

    function append(component) {
        var obj = single_scene_component.createObject(stack_viewer, {"cmpnt": component})
        list.append(obj)

        obj.show()
    }

    function back() {
        if(list.count == 0)
            return

        var obj = list.takeLast()
        obj.close()
    }

    function home() {
        for(var i=0; i<list.count-1; i++)
            list.takeAt(i).destroyItem()

        back()
    }
}

