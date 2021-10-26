import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    // Entity Enum controller
    EntityEnum {
        id: src
        Component.onCompleted: {
            updateProtoEntity(proto); // set proto entity
            context = int_root.context // set device context
        }
    }

    // View interface
    ComboBox {
        width: 200
        model: proto.values // list of all possible enum values
        currentIndex: proto.values.indexOf(src.value) // current value
        onCurrentIndexChanged: {
            src.value = proto.values[currentIndex];
        }
    }
}
