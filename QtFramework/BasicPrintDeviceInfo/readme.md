# Basic Print Device Info

This example in C++ shows how to print to console a list of all local devices and their info.

In this application we connect to local Displax Service through SessionManager.
Once connected, a Session object is emitted. A Session represents a Displax Service.
With this session, we are notified when devices are connected or removed from computer.

In SessionManager, at start method, we can set to discovery DisplaxServices across the network.
Then, for all DisplaxServices available in network will be emitted a Session object.

In this example we show how to connect to DisplaxService and list connected devices.
