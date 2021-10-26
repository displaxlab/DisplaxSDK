# Active Tags

Create a very simple Qml (GUI) application that shows device Active Tags.
It shows how to receive and handle active tags events, send color and vibration effects, and activate joystick mode.

This example shows an empty green field with a central spot. Let's call the central spot as joystick area.

When you put an active tag on sensor it will draw a small cylinder, orange and blue, on scene.
You can move the active tag across the sensor.
If the active tag enters on joystick area it emits a color and vibration effect.
Here you can lift the active tag, the camera changes position, and you will see the scene and lifted tag representation from another perspective.
When lifted, the active tag reproduces a color effect.
With the tag raised, the rotations, in relation to gravity vector, will be reflected in the scene.
Put active tag on sensor, outside joystick area, to disable joystick mode.

> :information_source: **Read first [Library and protocol specification.](../client-lib/readme.md) and [Network Device Info](../NetworkDeviceInfo/readme.md)**

