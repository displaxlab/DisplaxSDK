import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import com.displax.sdk

Item {
    id: int_root
    property var proto
    property var context
    anchors.fill: parent

    // Entity bool controller
    EntityBool {
        id: src
        Component.onCompleted: {
            updateProtoEntity(proto); // set proto entity
            context = int_root.context; // set device context
        }
    }

    // View interface
    Switch {
        onPositionChanged: {
            src.value = position === 1.0 ? true : false;
        }
        Component.onCompleted: {
            position = src.value ? 1.0 : 0.0;
        }
    }
}
