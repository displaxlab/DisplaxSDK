import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

Window {
    minimumWidth: 500
    maximumWidth: 500
    minimumHeight: 480
    visible: true
    property var device: null
    title: "Displax " + device.productName + ": " + device.serialNumber
    Material.theme: Material.Dark
    Material.accent: Material.DeepOrange

    function entityName2Human(s) {
        if (typeof s !== "string") return ""
        var ret = "";
        s.replace(/\./g, " ").replace(/_/g, " ").split(" ").forEach(function (i) {
            ret = ret + " " + i.charAt(0).toUpperCase() + i.slice(1)
        });
        return ret;
    }

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor

        Rectangle {
            x: 200
            y: 0
            width: parent.width-x
            height: parent.height
            color: Material.frameColor
        }

        ScrollView {
            anchors.fill: parent
            clip: true
            ScrollBar.horizontal.interactive: true
            ScrollBar.vertical.interactive: true

            Column {
                // Iterate for each device entity
                // for each valid entity create a label with entity name, and view handle to manipulate value.
                Repeater {
                    id: list_view
                    property int count : 0;
                    model: device.protoEntities

                    onItemAdded: {
                        if(item.visible)
                            item.odd = ++count%2==0;
                    }

                    Row {
                        y:2
                        height: 50
                        // Filter entity type: string, int, float, enum or bool
                        visible: modelData.type === "String" || modelData.type === "int" || modelData.type === "float"  || modelData.type === "Enum" || modelData.type === "bool"
                        property bool odd: false

                        Rectangle {
                            height: 50
                            width: 200
                            clip: true
                            color: odd ? Material.backgroundColor: Material.frameColor

                            Rectangle {
                                x: 10
                                y: (parent.height - height)/2
                                height: 10
                                width: 10
                                radius: 5
                                color:  odd ? Material.accentColor: Material.color(Material.accentColor, Material.Shade400)
                            }

                            Label {
                                x: 30
                                y: (parent.height - height)/2
                                font.pixelSize: 14
                                text: entityName2Human(modelData.name) // show a pretty name
                            }
                        }

                        Rectangle {
                            id: entity_view
                            width: 300
                            height: 50
                            color: odd ? Material.backgroundColor: Material.frameColor
                            Component.onCompleted: {
                                if(parent.visible)
                                { // load respective view handle
                                    var component = Qt.createComponent("EntityView_" + modelData.type + ".qml");
                                    if (component.status === Component.Ready)
                                        component.createObject(entity_view, { proto: modelData, context: device});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
