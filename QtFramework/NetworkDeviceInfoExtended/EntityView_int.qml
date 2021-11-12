import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    // Entity Int controller
    EntityInt {
        id: src

        Component.onCompleted: {
            updateProtoEntity(proto); // set proto entity
            context = int_root.context // set device context
        }

        onInitialisedChanged: {
            if(initialised && !dirty)
            {
                // If value is 0 or 1, loads bool switch
                // If is a tri value load special tri state switch
                // If value is between 0 and 100 loads slider
                // Otherwise loads a spin box.
                if(max-min===1)
                    my_loader.sourceComponent = switch1;
                else if(max-min===2)
                    my_loader.sourceComponent = switch2;
                else if(max-min<=100)
                    my_loader.sourceComponent = slider;
                else
                    my_loader.sourceComponent = spinBox;
            }
        }
    }

    Loader {
        id: my_loader
    }

    Component {
        id: switch1
        Switch{
            position: src.value-src.min
            onPositionChanged: {
                if(src.value != position + src.min)
                    src.value = position + src.min;
            }
        }
    }

    Component {
        id: switch2
        Switch {
            onPositionChanged : {
                var newPosition = src.value != 0 ? (src.value - src.min) / 2 : 0;
                if(newPosition != position)
                    src.value = position + src.min;
            }
            Component.onCompleted: {
                position = src.value != 0 ? (src.value - src.min) / 2 : 0;
            }
        }
    }

    Component {
        id: slider
        Slider {
            from: src.min
            value: src.value
            to: src.max
            onValueChanged: {
                if(src.value != value)
                    src.value = value;
            }
        }
    }

    Component {
        id: spinBox
        SpinBox {
            from: src.min
            value: src.value
            to: src.max
            editable: true
            onValueChanged: {
                if(src.value != value)
                    src.value = value;
            }
        }
    }
}

