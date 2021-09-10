import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQml 2.12
import QtQuick.Layouts 1.12
import com.displax.sdk 1.0
import com.displax.DeviceFrameTouch 1.0

Item {
    property var device;
    anchors.fill: parent
    id: root

    // Frame
    // Entity handle implemented in C++
    EntityFrame {
        id: srcFrame
        name: device.protoEntities.find(({type}) => type === "Frame").name
        context: device
    }

    // View to draw frame in QSGNode
    FrameViewer {
        anchors.fill: parent
        id: viewer
        image: srcFrame.value
        visible: device.valid
    }

    // Touches
    // Lets create Qml/Javascript handle to touchs

    // A generic Entity to handle touches
    EntityJsonValue {
        id: srcTouches
        name: device.protoEntities.find(({type}) => type === "Touches" || type === "TouchEntities").name
        context: device
    }

    // Generic Entity to handle touch space size
    EntityJsonValue {
        id: srcTSpace
        name: device.protoEntities.find(({name}) => name === "flogs" || name === "frame.size.logical").name
        context: device
        property real w: value ? value[0] : 1
        property real h: value ? value[1] : 1
    }

    Item {
        // Transform to touch space size
        transform: Scale { xScale: root.width/srcTSpace.w; yScale: root.height/srcTSpace.h}

        Repeater {
            // Populate touches with visual elements
            model: srcTouches.value? srcTouches.value.entities: []
            Rectangle {
                x: modelData.x - 50
                y: modelData.y - 50
                width: 100
                height: 100
                radius: 50
                color: Material.accentColor
                opacity: 0.5
            }
        }
    }
}
