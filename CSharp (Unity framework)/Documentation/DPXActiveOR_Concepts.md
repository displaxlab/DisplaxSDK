## DPXActiveOR library - Concepts

### Base requirements

An Active OR system needs a Displax Skin sensor, an Ultra controller coupled with an add-on module, a host PC with Displax service daemon installed and ActiveOR enabled, and, at least, one ActiveOR tag paired with the add-on module+controller.

To pair tags to the system (add-on+ultra controller), please use the DISPLAX Connect software. You can also use the Connect software to set the tag ID (which will be reported/used by DPXActiveOREngine to identify the tags).

Having a successful connection with an enabled Active OR controller, the DPXActiveOR library is ready to deliver tag events to the other app objects and send actions to connected/paired tags to that controller.

### Tag states
A paired tag can be in a inactive state (when is sleeping, which happens when the tag is idle for 6-10 seconds, if in normal mode), or in active state. 

When in active state, the tag reports it's information to the controller (if is in sensor range, battery level, was tapped or not). 

### In range, in contact, "out-of-range" coordinates
If the tag is not touching the sensor, the controller reports to the Displax Service daemon "out-of-range" coordinates (usually, a 65535 value in x and y position). If the tag is touching the sensor, the controller updates and reports to the Displax Service daemon the position, in_contact, rotation and other information.

### Communication slots 
Although up to 84 ActiveOR tags can be paired to one add-on + controller, the system can only report up to 6 active tags - the controller has 6 slots, supporting simultaneous communication with up to 6 active tags - (if a 7th tag enters active state, it won't be detected as active, unless one or more of the previously active tags enters inactive state).

### Normal and Joystick modes

#### Normal mode
Typically, and by default, a tag is in "normal mode". In this mode, the tag is detected being in/out of the sensor, it's position and rotation (relative to the sensor) is calculated.

In normal mode, the system can handle up to 6 active tags simultaneously.

#### Joystick mode

In joystick mode, the system can handle one active tag only. 

The tag should be manipulated away from the sensor, and it's x-y-z axis coordinates will be reported. 

To enter joystick mode, an action to enter joystick mode should be sent with the desired tag ID as parameter. 



