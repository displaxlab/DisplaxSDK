import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import com.displax.sdk 1.0

Item {
    id: int_root
    property var proto;
    property var context;
    anchors.fill: parent

    // Entity float controller
    EntityFloat {
        id: src

        Component.onCompleted: {
            updateProtoEntity(proto); // set proto entity
            context = int_root.context // set device context
        }

        onInitialisedChanged: {
            if(initialised && !dirty)
            {
                // For normalized values load slider, otherwise load spin box.
                if(max-min===1)
                    my_loader.sourceComponent = slider;
                else
                    my_loader.sourceComponent = spinBox;
            }
        }
    }

    Loader {
        id: my_loader
    }

    // View interface for normalized values
    Component {
        id: slider
        Slider {
            from: src.min
            value: src.value
            to: src.max
        }
    }

    // Default view interface
    Component {
        id: spinBox
        SpinBox {
            from: src.min
            value: src.value
            to: src.max
            editable: true
        }
    }
}
