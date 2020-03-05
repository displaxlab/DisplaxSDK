import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    EntityEnum {
        id: src
        Component.onCompleted: {
            updateProtoEntity(proto);
            context = int_root.context
        }
    }

    ComboBox {
        width: 200
        model: proto.values
        currentIndex: proto.values.indexOf(src.value)
        onCurrentIndexChanged: {
            src.value = proto.values[currentIndex];
        }
    }
}
