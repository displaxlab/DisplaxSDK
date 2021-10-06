import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    EntityInt {
        id: src

        Component.onCompleted: {
            updateProtoEntity(proto);
            context = int_root.context
        }

        onInitialisedChanged: {
            if(initialised && !dirty)
            {
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
                src.value = position+src.min;
            }
        }
    }

    Component {
        id: switch2
        Switch {
            position: (src.value-src.min)/2
            onPositionChanged : {
                src.value = position+src.min;
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
                src.value = value;
            }
        }
    }
}

