import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

Item {
    height: 100
    width: 70

    Rectangle {
        height: parent.height
        width: parent.width
        color: Material.accentColor
        radius: 5

        Rectangle {
            x: 5
            y: 5
            width: parent.width-10
            height: parent.height-10
            color: Material.backgroundColor

            Rectangle {
                x: 7
                y: 10
                width: parent.width-14
                height: 7
                color: Material.accentColor
            }

            Rectangle {
                x: 7
                y: 21
                width: parent.width-14
                height: 7
                color: Material.accentColor
            }
        }
    }
}
