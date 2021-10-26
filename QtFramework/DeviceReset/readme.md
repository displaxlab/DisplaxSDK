# Device Reset

Create a very simple application that discovers all devices and restarts them.

In this application we connect to local Displax Service through SessionManager.
Once connected, a Session object is emitted. A Session represents a Displax Service.
With this session, we are notified when devices are connected to computer.
In this example, for each new device we send a restart request.

In this example we show how to create a basic entity, function, from device and execute it.
