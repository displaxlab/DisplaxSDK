import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

Item {
    width: 150
    height: width*0.6
    property var device: null

    Rectangle {
        id: screen
        width: parent.width
        height: parent.height*0.9
        color: Material.accentColor
        radius: 10

        Rectangle {
            x: 7
            y: 7
            width: parent.width-14
            height: parent.height-14
            color: Material.backgroundColor
        }

        Label {
            y: (parent.height - height)/2
            x: (parent.width-width)/2
            font.bold: true
            font.pixelSize: 10
            color: Material.accentColor
            text: device.productName
        }

    }
    Rectangle {
        x: parent.width/3
        y: parent.height*0.9
        width: parent.width/3
        height: parent.height-y
        color: Material.accentColor
    }
}
