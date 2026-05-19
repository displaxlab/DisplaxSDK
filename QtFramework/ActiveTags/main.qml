import Qt3D.Core
import Qt3D.Extras
import Qt3D.Input
import Qt3D.Render
import QtQml.Models
import QtQuick
import com.displax.sdk

Entity {
    id: root

    SessionManager {
        id: sm
        Component.onCompleted: sm.start(true) // We only want local DisplaxService / session

        // Handle sessions.
        property var ins: Instantiator {
            id: menu_device_sessions_instantiator
            model: sm.sessions
            delegate: Item { // For each session found
                id: menu_device_session
                property var session: modelData

                Instantiator {
                    model: modelData.devices
                    delegate: Item { // For each device from session
                        property var device: modelData
                    }
                    onObjectAdded: (index, object) => { // Select only local session/device
                        if (session.isLocal && device_obj === null)
                            device_obj = object.device;
                    }
                    onObjectRemoved: (index, object) => {
                        if (device_obj === object.device)
                            device_obj = null;
                    }
                }
            }
            onObjectRemoved: (index, object) => {
                if (device_obj && !device_obj.valid)
                    device_obj = null;
            }
        }
    }

    property var device_obj: null
    property var scene: null

    onDevice_objChanged: {
        if (scene) {
            scene.destroy();
            scene = null;
        }

        if (device_obj && device_obj.valid) {
            // Create simple scene for device arrived.
            var component = Qt.createComponent("scene.qml");
            if (component.status === Component.Ready)
                scene = component.createObject(root, {
                    device: device_obj
                });
        }
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _window.width / _window.height
        nearPlane: 1000
        farPlane: 10000.0
        position: scene ? (scene.joystick_tag_up ? Qt.vector3d(scene.device_width / 2, scene.device_width / (3 * Math.sin(Math.PI / 4)), 2 * scene.device_height) // Joystick mode
            : Qt.vector3d(scene.device_width / 2, scene.device_width / (2 * Math.sin(Math.PI / 4)), scene.device_height / 2))// Normal mode
        : Qt.vector3d(0.0, 5000.0, 0.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: scene ? Qt.vector3d(scene.device_width / 2, 0.0, scene.device_height / 2) : Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
        camera: camera
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings {}
    ]
}
