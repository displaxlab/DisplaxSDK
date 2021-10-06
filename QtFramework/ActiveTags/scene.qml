import QtQuick 2.12 as QQ2
import QtQml.Models 2.12 as QQM
import com.displax.sdk 1.0
import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.14

Entity {
    id: sceneRoot
    property var device;
    property alias device_width: srcTSpace.w
    property alias device_height: srcTSpace.h
    property alias joystick_tag_up: srcTouches.joystick_tag_up

    // Lets create Qml/Javascript handle to Active Tags

    // A generic Entity to handle touches and Active Tags
    EntityTouchEvents {
        id: srcTouches
        name: device.protoEntities.find(({type}) => type === "TouchEntitiesActive").name
        context: device

        property bool joystick_mode: false
        property bool joystick_tag_up: false
        property int joystick_identifier: 0

        // Control joystick mode
        onValueChanged: {
            if(value.entities == null) {
                joystick_mode = false;
                joystick_tag_up = false;
                joystick_identifier = 0;
                return;
            }

            var arr = srcTouches.isInside(device_width/2, device_height/2, 300);

            if(joystick_mode === true) {
                var obj1 = value.entities.find(({identifier}) => identifier === joystick_identifier);
                var obj2 = arr.find(({identifier}) => identifier === joystick_identifier);
                // If left the joystick area, disable joystick mode.
                if(obj1 == null || (obj1 && obj1.in_contact && obj2 == null)) {
                    srcTouches.setEntityProperty({"action": "joystick",
                                                     "enable": false,
                                                     "identifier": joystick_identifier});
                    joystick_mode = false;
                    joystick_tag_up = false;
                    joystick_identifier = 0;
                    return;
                }
                // If tag rise, activate.
                if(!obj1.in_contact && !joystick_tag_up) {
                    joystick_tag_up = true;
                    // Send color effect.
                    srcTouches.setEntityProperty({"action": "color_effect",
                                                  "color": "white",
                                                  "effect": 2,
                                                  "time": 200,
                                                  "cycles": 1,
                                                  "tag_id": obj1.tag_id});
                    return;
                }
            } else {
                for(var i in arr) {
                    // If one tag arrive to joystick area, activate joystick mode.
                    if(arr[i].type === 7 && arr[i].in_range === true) {
                        srcTouches.setEntityProperty({"action": "joystick",
                                                         "enable": true,
                                                         "tag_id": arr[i].tag_id});
                        joystick_mode = true;
                        joystick_identifier = arr[i].identifier;
                        // On enter, send color effect
                        srcTouches.setEntityProperty({"action": "color_effect",
                                                      "color": "blue",
                                                      "effect": 1,
                                                      "time": 500,
                                                      "cycles": 2,
                                                      "tag_id": arr[i].tag_id});
                        // and vibration effect
                        srcTouches.setEntityProperty({"action": "vibration_effect",
                                                      "level": "50",
                                                      "time_on": 100,
                                                      "time_off": 200,
                                                      "cycles": 2,
                                                      "tag_id": arr[i].tag_id});
                        return;
                    }
                }
            }
        }
    }

    // Generic Entity to handle touch space size
    EntityJsonValue {
        id: srcTSpace
        name: device.protoEntities.find(({name}) => name === "flogs" || name === "frame.size.logical").name
        context: device
        property real w: value ? value[0] : 1
        property real h: value ? value[1] : 1
    }

    // Set point light.
    Entity{
        components: [
            PointLight {
                color: "white"
            },
            Transform {
                translation: Qt.vector3d(device_width/2, 10000, device_height/2)
            }

        ]
    }

    // Draw walls
    PhongMaterial {
        id: wall_material
        specular: "white"
        ambient: "grey"
        diffuse: "lightgrey"
    }

    CuboidMesh {
        id: a_wall_mesh
        xExtent: device_width
        yExtent: 100
        zExtent: 100
    }

    CuboidMesh {
        id: b_wall_mesh
        xExtent: 100
        yExtent: 100
        zExtent: device_height
    }

    Entity {
        Transform {
            id: a_wall_transform
            translation: Qt.vector3d(device_width/2, 50, 0)
        }
        components: [a_wall_mesh, wall_material, a_wall_transform]
    }

    Entity {
        Transform {
            id: aa_wall_transform
            translation: Qt.vector3d(device_width/2, 50, device_height)
        }
        components: [a_wall_mesh, wall_material, aa_wall_transform]
    }

    Entity {
        Transform {
            id: b_wall_transform
            translation: Qt.vector3d(0, 50, device_height/2)
        }
        components: [b_wall_mesh, wall_material, b_wall_transform]
    }

    Entity {
        Transform {
            id: bb_wall_transform
            translation: Qt.vector3d(device_width, 50, device_height/2)
        }
        components: [b_wall_mesh, wall_material, bb_wall_transform]
    }

    // Draw floor
    Entity {
        components: [
            PhongMaterial {
                specular: "black"
                ambient: "darkgreen"
                diffuse: "green"
            },
            PlaneMesh {
                width: device_width
                height: device_height
                mirrored: true
            },
            Transform {
                translation: Qt.vector3d(device_width/2, 0, device_height/2)
            }
        ]
    }

    // Draw joystick area
    Entity {
        components: [
            PhongMaterial {
                specular: "black"
                ambient: "lightgreen"
                diffuse: "lightgreen"
            },
            CylinderMesh {
                length: 10
                radius: 400

            },
            Transform {
                translation: Qt.vector3d(device_width/2, 5, device_height/2)
            }
        ]
    }


    NodeInstantiator {
        // Populate active tags with visual elements
        model: srcTouches.value? srcTouches.value.entities: []
        Entity{
            enabled: modelData.type === 7
            Entity{
                components: [
                    CylinderMesh {
                        length: 20
                        radius: 290
                    },
                    PhongMaterial {
                        ambient: "darkorange"
                        diffuse: "orange"
                    },
                    Transform {
                        translation: Qt.vector3d(0, 60, 0)
                    }
                ]
            }
            Entity{
                components: [
                    CylinderMesh {
                        length: 20
                        radius: 300

                    },
                    PhongMaterial {
                        ambient: "blue"
                        diffuse: "blue"
                    },
                    Transform {
                        translation: Qt.vector3d(0, 40, 0)
                    }
                ]
            }
            components: [
                CylinderMesh {
                    length: 60
                    radius: 300

                },
                PhongMaterial {
                    ambient: "saddlebrown"
                    diffuse: "peru"
                },
                Transform {
                    property var joystick: srcTouches.joystick_tag_up && modelData.identifier === srcTouches.joystick_identifier
                    translation: joystick
                                 ? Qt.vector3d(device_width/2, 300, device_height/2)
                                 : Qt.vector3d(modelData.x, 30, modelData.y)

                    // Choose only one rotation approach.

                    // Rotation in hemisphere:
                    // rotationX: joystick ? modelData.tilt_x * 180 / Math.PI : 0
                    // rotationZ: joystick ? modelData.tilt_y * 180 / Math.PI : 0

                    // Rotation in full globe:
                    rotation: joystick ? fromAxisAndAngle(Math.sin(modelData.tilt_x),
                                                          0,
                                                          Math.sin(modelData.tilt_y),
                                                          90 - modelData.tilt_z)
                                       : "1, 0, 0, 0"
                }
            ]
        }
    }
}
