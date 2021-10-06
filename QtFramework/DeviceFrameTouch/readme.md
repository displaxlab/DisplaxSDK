# Device Frame Touch

Create a very simple Qml (GUI) application that shows device frame and touches.
You can select devices in menu style.
In this example we also show two approaches to handle service entities.

To handle frame we implement EntityFrame and FrameViewer (subclass of QSG node)
in C++. In Qml declare both and connects with each other. With this hybrid approach we have more
control and efficiency but is more verbose, complex and error prone.

For touches we use a EntityJsonValue that provide Json receive by service. With that we can implement
a pure Qml/Javascript handler. It's very simple, objective and clear.

When it comes to building handler to service entities we have two options. One, is doing hard work in
C++ and bind to Qml. Other, is take advantage of QJsonValue and implement all in Qml/Javascript,
making all clear and simple.

Now it's up to you to choose the best approach according to your context and needs.
