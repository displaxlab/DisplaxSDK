## DPXActiveOREngine class
The DPXActiveOR library includes a frontend MonoBehavior class, DPXActiveOREngine, which should be attached to a game object and, if possible, be persistent through the application runtime existence (to maintain the system connection and stable communication).

It handles the necessary procedures to automatically connect with the first active OR enabled Ultra controller (or a specific Ultra controller, if it's serial number is defined in DPXActiveOREngine properties - see "How to use it" section).

DPXActiveOREngine receives the information updates from the Displax Service daemon to which is connected to and process them, at least in two ways:
- update it's internal tag information
- notifies/trigger any event handlers attached to it

So, if any object class in the app wants to receive tag information/updates, it should register to the desired event (see below the available events).

### Properties

- **remoteHost** - Host IP address where Displax Service daemon is installed and running. Typically, localhost (IP address 127.0.0.1)

    public string remoteHost = "127.0.0.1";

- **remotePort** - Host port where Displax Service daemon is listening. Typically, 45001.

    public int remotePort = 45001;

- **useFirstAvailableController** - If true, will automatically use the first add-on + controller system reported by the Displax Service daemon. If false, will check for a system with serial number _controllerSerialNumber_. True as default.
    
    public bool useFirstAvailableController = true;
    
- **controllerSerialNumber** - If _useFirstAvailableController_ is false, this property should be filled with the Ultra controller serial number to be connected.
    
    public string controllerSerialNumber;

- **translateTagCoordsToScreen** if true, the tags coordinates will be translated to current Screen resolution. True by default.

    public bool translateTagCoordsToScreen = true;

- **usePlayerWindowResolution** (this property is active only if _translateTagCoordsToScreen is set to true). if true, the tags coordinates will be translated in relation with the app window resolution. If false, the tags coordinates will be translated in relation to the actual full screen resolution. True by default.

    public bool usePlayerWindowResolution = true;

- **invertXCalibratedCoords** - Depending on app necessities, the reported tag X coordinates can be inverted by setting this property to true.
 
    public bool invertXCalibratedCoords = true;

- **invertYCalibratedCoords** - Depending on app necessities, the reported tag Y coordinates can be inverted by setting this property to true.

    public bool invertYCalibratedCoords = false;

  **invertRotation** - Depending on app necessities, the reported tag rotation angle can be inverted by setting this property to true.
  
    public bool invertRotation = false;

- **onlyValidTagIDs** - if a tag is detected on the sensor, but the BLE communication is not established (tag not paired and/or exiting sleep state), it is reported with an invalid ID (greater than 65535). These tag reports can be filtered out if this property is set to True (default). 
    public bool onlyValidTagIDs = true;

- **ignoreTapWhenTagIsVibrating** - this property should be true to avoid false tap reports which may be generated with certain vibration effect configurations. True by default. 

    public bool ignoreTapWhenTagIsVibrating = true;

- **tag_prefab** - a tag game object can be automatically created if this property is defined with a prefab reference. Check the example for more details. 

    public GameObject tag_prefab;

- **isConnected** - read-only property. Returns true if system is connected to a Displax Service daemon with a valid add-on+controller. Returns false otherwise.
    
    public bool isConnected { get }

### Events

DPXActiveOREngine provides a simple event handler system, based on reference pointer delegation.

To add an handler to a specific event, add the handler method reference pointer to the event delegate.

Example: 

```
{
    DPXActiveOREngine.onTagUpdated -= ProcessTagData;
    DPXActiveOREngine.onTagUpdated += ProcessTagData;
}
```


The available events are:
- **onConnected**

Triggered when the system (add-on + controller) is connected.

Delegate definition:

```
    public delegate void OnConnected();
    public static event OnConnected onConnected;
```

- **onDisconnected**

Triggered when the system (add-on + controller) is disconnected.

Delegate definition:

```
    public delegate void OnDisconnected();
    public static event OnDisconnected onDisconnected;
```

- **onTagUpdated**

Triggered when there's an active tag report/update available.

Delegate definition:

```
    public delegate void OnTagUpdated(DPXActiveTag tagData);
    public static event OnTagUpdated onTagUpdated;
```

This event defines an input parameter sent (_tagData_) to each associated handler method. _tagData_ is an _DPXActiveTag_ object.

For advanced/in-depth applications, inner classes events can be used, such as DPXSessions class. Please check the classes code for further information.

### Methods

- **RemoveTagObjects** 

This function should be called prior to scene change

```
    public void RemoveTagObjects()
```
- **SetJoystickMode**

Function to set joystick mode. In joystick mode, only tag with tag_id will be used (tag id should be provided as parameter).

Parameters:

`tag_id` - tag ID to be used in joystick mode

```
    public bool SetJoystickMode(int tag_id)
```
- **SetNormalMode**

Function to set normal operation  mode. 

```
    public bool SetNormalMode()
```
- **SendTagColorEffect**

Function to send color effect to tag.

Parameters:

`tag_id` - tag ID

`color` - color

`effect` - effect type, selected from a pre-set list of effects, defined by enum _ActiveTagColorEffect { FadeToColor, Blink, ShortPulse, TriangularWave, ToothSawWave, HeartBeat }_

`cycles` - effect execution number of cycles

`time` - cycle time, in milisseconds. Each effect cycle will take _time_ milisseconds

`sendImmediate` - to send immediately the effect action, or add it to the actions queue. True by default, to send effects immediately.

```
    public bool SendTagColorEffect(int tag_id, Color color, ActiveTagColorEffect effect, int cycles, int time, bool sendImmediate = false)
```
- **SendTagVibrationEffect**

Function to send vibration effect to tag.

Parameters:

`tag_id` - tag ID

`cycles` - effect execution number of cycles

`time` - cycle time, in milisseconds. Each effect cycle will take _time_ milisseconds

`level` - vibration strength. Ranges from 0 (no vibration) to 100 (strongest vibration).

`time_on` - duration of vibration on each cycle. Followed by a _time_off_ duration.

`time_off` - duration of no-vibration on each cycle. Executed right after _time_on_ vibration period on each cycle.

`sendImmediate` - to send immediately the effect action, or add it to the actions queue. True by default, to send effects immediately.

```
    public bool SendTagVibrationEffect(int tag_id, int cycles, int level, int time_on, int time_off, bool sendImmediate = false)
```
