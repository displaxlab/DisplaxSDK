# Network Device Info Extended

Extend example Network Device Info to provide basic device control.
In this example we show how to handle and manipulate basic entity types, like int, bool, float, Enum and String.

When the application catches a device, it iterates over device entities, proto entities.
For each proto entity, if its an int, bool, float, Enum or String,
creates a QML view controller to handle device entity. This handle allows to view and change entity device.
See DeviceWindow.qml for this implementation.
