# Client Library

This library abstracts all communication and the entities synchronization process with the remote service.
Allows a fast implementation and code reuse.

The protocol communication between service and client relies on TCP/IP with JSON Qt binary format.

> :warning: **Displax Service isn't compatible with Qt 5.15**, so use until Qt 5.14.

## Communication

This module discovers remote services and establishes a connection with them.

For each new TCP/IP connection we must start to send a "connection_info" datagram to service.

```json
{
    "frame_type": "connection_info",
    "server_port": 0,
    "type": "DesktopConnect",
    "id": my_id,
    "communication_port": my_tcp_port
}
```

`my_id` contains a unique ID that identifies your connection.

`my_tcp_port` is your tcp port that you are using.
This is for verification purposes only, since we can have multiple clients on the same machine.

At this point, if everything went well, connection is established and you can start to communicate.

## Session and Requests

For each new TCP/IP connection the Session Manager creates an associated Session.

A Session represents a remote service and handles requests and responses.
To communicate with the service we can send two types of requests: actions or cli.
In response, we receive reactions (for actions), notifications and cli.
For now, in this document, we will omit cli.

Actions request contains a list of actions.

```json
{
    "actions": {
        "list": [
            action,
            ...
        ]
    }
}
```

Each `action` is a object and must contain key `key`.
Optionally can contain key `id`, for the service this means nothing, but will return it in the
correspondent reaction.
This is useful for session match actions with reactions.

The service does not guarantee a processing order for actions, it is asynchronous and unordered.
So, it's possible that some new actions are processed before old actions. Keep that in mind.
If you want to ensure some order you must wait for reactions to send new actions.

In response to an action the service replies with a reaction.

```json
{
    "reactions": {
        "list": [
            reaction,
            ...
        ]
    }
}
```

If an error occours, a reaction is sent with an error key and the associated id, if exists.

Other type of response from service is notifications.
If you register to listen some entity, when it is changed in service, the service notifies you
through notification.

## Actions and Reactions

### Session Protocol Version

The version action should be the first action to send to service.

```json
{
    ...
    {
        "key": "version",
        "id": 1
    }
    ...
}
```

In reaction, we receive something like this:

```json
...
{
    "key": "version",
    "id": 1,
    "value": "Session Manager 1.0.0"
}
...
```

The protocol is only compatible in the same major revision.
So, before you start communicating you must make sure you’re dealing with the same protocol.

### Action Notifications

Sometimes it is useful that when some entity changes in the service we get notified for that change.
For this, we must inform the service that we want to be notified.
When you don't want to be notified any more just unregister the entity.

```json
{
    ...
    {
        "key": "notifications",
        "id": some_id,
        "register": [
                {
                    "key": "entity_name"
                },
                {
                    "key": "entity_name",
                    "device": "device_serial_number"
                },
                ...
        ],
        "unregister": [
                {
                    "key": "entity_name"
                },
                {
                    "key": "entity_name",
                    "device": "device_serial_number"
                },
                ...
        ]
    }
    ...
}
```

Only entities can be observable.

### Devices All

This action returns a list of devices.
Each device has product name, product id, serial number, firmware version, type and entities prototype.
A entity prototype is a entity declaration that contains name, type, description and other relevant keys

Reaction example:

```json
...
{
    "key": "devices_all",
    "id": some_id,
    "value": [
            {
                "product_id": 2,
                "product_name": "Skin Ultra",
                "version": "2.5.0",
                "device_type": "Hid",
                "serial_number": "02030255662155601",
                "views": [
                    {
                        "view": "Displax_Connect",
                        "value": [
                                {
                                    "name": "dreset",
                                    "type": "Function",
                                    ...
                                },
                                {
                                    "name": "gain",
                                    "type": "int",
                                    "domain": "[0..7]",
                                    "description": "some description..."
                                },
                                ...
                        ]
                    }
                ]
            },
            ...
    ],
}
...
```

### Regular Entity

To read an entity value we must send a action:

```json
{
    ...
    {
        "key": "entity_name",
        "id": some_id,
        "device": "device_serial_number"
    }
    ...
}
```

Or, set value to entity:

```json
{
    ...
    {
        "key": "gain",
        "id": some_id,
        "device": "device_serial_number",
        "value": 3
    }
    ...
}
```

The reaction and notification structure is similar to a set value action.

We call this type of action *activate entity*.


## Entities Types

This section shows a resume for entities types. For more detail see their implementation.

### Int and Float

These entity types represent a number value.
It contains the key `domain` that bounds values.

Entities like gain, threshold or shield are int.

### Bool

This entity type represents a boolean value.

### String

This entity type represents a string value.

### Enum

This entity type represents a enum value.
It contains *values* key that shows all possible values.

### Function

Function allows to execute some entity function on service.
A Function accepts arguments, a list of QVariant, and returns a QVariant value.

In action, instead of the key `value` use `arguments`.
The reaction return *value* key.

### Matrix3x3

This entity represents geometric calibration.
It consists of nine float values (homogeneous matrix 2D).
So, value is a array of nine floats.

### Frame

Frame entity contains a png image. It is read only.
It represents a device acquisition snapshot.

### Touches and TouchEntities

This entity gets touches from device.
Its value is an object with two keys: `or-spec` (object recognition version) and entities.
Entities are an array of touch entities. Each touch entity contain:

* `type` - see Displax TUIO specification
* `subtype` - see Displax TUIO specification
* `identifier` - unique id
* `x`
* `y`
* `x_calibrated`
* `y_calibrated`
* `width`
* `height`
* `radius`
* `pressure`
* `rotation`
* `rotation_calibrated`
* `tilt_x`
* `tilt_y`
* `color`
* `barrel`
* `eraser`
* `nested_entities` (an array of touch entities)

Only type, subtype, identifier, x and y are mandatory. Others are optional.
Angular fields are in radians.

### TouchEntitiesActive

TouchEntitiesActive extends TouchEntities adding active tags support.
All new fields are optional:

* `tilt_z`
* `name`
* `battery_level`
* `tap`
* `double_tap`
* `in_range`
* `in_contact`
* `tag_id`

In addition, you can send effects or change certain properties to active tags.
For that, you must send a JSON object with pair, key value, `actions` array.
For each array element (object), this must contain a `action` and `tag_id`. An action can be:

* `color_effect` sends a color effect to respective active tag
    * `color` string color (SVG color names) or #rrggbb format.
    * `effect` can be one of the following values:
        * 0 fade
        * 1 blink
        * 2 short pulse
        * 3 triangular wave
        * 4 tooth saw wave
        * 5 heart beat
    * `time` duration of effect cycle in ms
    * `cycles` repeat effect
* `vibration_effect` sends a vibration effect for respective active tag
    * `level` a integer between 0 and 100
    * `time_on` dutty cycle on (ms)
    * `time_off` dutty cycle off (ms)
    * `cycles` repeat effect
* `joystick`
    * `enable` true or false. Enable joystick mode for respective tag. Only one tag can be in joystick mode.

An example:

```json
{
    ...
    {
        "actions": [
            {
                "action": "color_effect",
                "tag_id": some_tag_id,
                "color": "red",
                "effect": 2,
                "time": 600,
                "cycles": 3
            },
            {
                "action": "vibration_effect",
                "tag_id": some_tag_id,
                "level": 80,
                "time_on": 400,
                "time_off": 750,
                "cycles": 2
            },
            {
                "action": "joystick",
                "tag_id": some_tag_id,
                "enable": "true"
            }
        ],
        ...
    }
    ...
}
```

### DeviceSettings

This entity provide mechanisms to save, download, upload, set default settings.

* download - download settings binary
* upload - upload settings binary (this does not save settings)
* set default - reset settings
* save - force permanent save settings

### Devices List

This entity holds a list with active device in service.
It is useful to notify from device insertion or removal.

Typically, from `device_list` notification request a `devices_all`.


