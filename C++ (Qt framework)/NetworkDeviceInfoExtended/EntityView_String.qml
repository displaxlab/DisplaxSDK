import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    EntityString {
        id: src
        Component.onCompleted: {
            updateProtoEntity(proto);
            context = int_root.context
        }
    }

    Label {
        x: 5
        y: (parent.height-height)/2
        text: src.value
    }
}
