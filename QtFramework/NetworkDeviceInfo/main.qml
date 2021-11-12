import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQml 2.14
import QtQuick.Layouts 1.14
import com.displax.sdk 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 640
    minimumHeight: 480
    title: qsTr("Network Device Info")
    Material.theme: Material.Dark
    Material.accent: Material.Indigo

    // Instantiate Displax Session Manager
    SessionManager{
        id: sm
        Component.onCompleted: sm.start(false);
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.interactive: true
        ScrollBar.vertical.interactive: true
        contentHeight: my_computers_col.height

        Label{
            id: networkTitle
            color: Material.frameColor
            font.pixelSize: 25
            font.bold: true
            padding: 10
            text: "Network"
        }

        Column {
            y: networkTitle.height
            id: my_computers_col
            padding: 5
            spacing: 10
            Repeater {
                model: sm.sessions
                // for each Session
                Rectangle {
                    x: 30
                    id: item_data
                    width: computers_elem.width
                    height: 130
                    radius: 5
                    color: Material.frameColor

                    Row {
                        id: computers_elem
                        spacing: 50
                        padding: 5
                        Item{
                            id: pc_desc
                            width: 100
                            height: 120

                            ComputerIcon{
                                id: icon
                                x: (parent.width-width)/2
                                z: 1
                            }

                            Label{
                                y: 120 - height
                                x: (parent.width-width)/2
                                id: pc_title
                                text: modelData.isLocal ? "localhost": modelData.url.replace("::ffff:","").split(":")[0]
                            }
                        }

                        Repeater {
                            model: modelData.devices
                            // For each device
                            Item {
                                width: 150
                                height: 120

                                Canvas {
                                    x: -(index==0? 65: 165)
                                    width: parent.width*2;
                                    height: parent.height
                                    contextType: "2d"

                                    Path {
                                        id: myPath
                                        startX: 0; startY: 90
                                         PathLine { x: 34+(index==0? 65: 165); y: 90 }
                                         PathArc {
                                                x: 64+(index==0? 65: 165); y: 75
                                                radiusX: 30; radiusY: 20
                                                direction: PathArc.Counterclockwise
                                            }
                                    }

                                    onPaint: {
                                        context.strokeStyle = Material.backgroundDimColor;
                                        context.lineWidth = 3;
                                        context.path = myPath;
                                        context.stroke();
                                    }
                                }

                                SensorIcon{
                                    width: 128
                                    label: modelData.productName
                                }

                                Label{
                                    id: my_label
                                    y: 120 - height
                                    x: (parent.width-width)/2
                                    text: "SN: " + modelData.serialNumber
                                }
                            }
                        }
                    }

                    Canvas {
                        x: -30
                        y: index==0? 0: -100
                        width: 30;
                        height: index==0?130:230
                        contextType: "2d"
                        Path {
                            id: myPath2
                            startX: 15; startY: 0
                             PathLine { x: 15; y: (index==0?0:100) + 35 }
                             PathArc {
                                    x: 30; y: (index==0?0:100) + 65
                                    radiusX: 15; radiusY: 30
                                    direction: PathArc.Counterclockwise
                                }
                        }

                        onPaint: {
                            context.strokeStyle = Material.backgroundDimColor;
                            context.lineWidth = 3;
                            context.path = myPath2;
                            context.stroke();
                        }
                    }

                }
            }
        }
    }
}
