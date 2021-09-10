import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQml 2.12
import QtQuick.Layouts 1.12
import com.displax.sdk 1.0

ApplicationWindow {
    id: root
    visible: true
    title: qsTr("DeviceFrameTouch")
    visibility: ApplicationWindow.Maximized

    minimumWidth: 640
    minimumHeight: 480

    Material.theme: Material.Dark
    Material.accent: Material.Indigo

    SessionManager {
        id: sm
        Component.onCompleted: sm.start(false);
    }

    property var device_obj: null
    property var device_view: null;

    // Populate Menu with sessions and devices
    menuBar: MenuBar {
        id: menu_bar
        Menu {
            title: qsTr("&File")

            Menu {
                id: menu_device_sessions
                title: qsTr("Devices")
                enabled: menu_device_sessions_instantiator.count > 0

                Instantiator {
                    id: menu_device_sessions_instantiator
                    model: sm.sessions
                    delegate: Menu {
                        id: menu_device_session
                        title: modelData.isLocal ? qsTr("localhost") : modelData.url
                        property var session: modelData

                        Instantiator {
                            model: modelData.devices
                            delegate: MenuItem {
                                property var device: modelData
                                text: modelData.productName + ": " + modelData.serialNumber
                                onTriggered: {
                                    device_obj = device
                                }
                            }
                            onObjectAdded: {
                                menu_device_session.insertItem(index, object)
                                if(device_obj === null)
                                    device_obj = object.device
                            }
                            onObjectRemoved: {
                                menu_device_session.removeItem(object)
                                if(device_obj === object.device)
                                    device_obj = null
                            }
                        }
                    }
                    onObjectAdded: {
                        menu_device_sessions.insertMenu(index, object)
                    }
                    onObjectRemoved: {
                        menu_device_sessions.removeMenu(object)
                        if(device_obj && !device_obj.valid)
                            device_obj = null
                    }
                }
            }

            MenuSeparator { }
            MenuItem {
                text: qsTr("&Quit")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&About"); onTriggered: about.open() }
        }
    }

    Dialog {
        id: about
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        width: 450
        modal: true
        title: "About"
        standardButtons: Dialog.Ok
        contentItem: Label {
            text: "Application example to show device frame and touches."
        }
    }

    Item {
        id: main_view
        anchors.fill: parent
    }

    onDevice_objChanged: {
        if(device_view)
        {
            device_view.destroy();
            device_view = null;
        }
        if(device_obj && device_obj.valid)
        {
            var component = Qt.createComponent("DeviceView.qml");
            if (component.status === Component.Ready)
                device_view = component.createObject(main_view, { device: device_obj});
        }
    }
}
