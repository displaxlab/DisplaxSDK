## Data Classes

An active tag event delivers to the registered handlers a DPXActiveTag object, defined in DPXActiveTag class.

###  DPXActiveTag

```
public class DPXActiveTag
{
    public int tag_id;              // tag ID
    public int battery_level;       // Battery level. Range: 0-100
    public bool double_tap;         // not used
    public int height;              // tag height. fixed value 200
    public int identifier;          // tag identifier (usually used in TUIO implementation) 
    public bool in_contact;         // True if tag is in contact with sensor. False otherwise.
    public bool in_range;           // True if tag is detecting the sensor is near. False otherwise.
    public string name;             // Tag label, defined in Displax Connect
    public int pressure;            // not used
    public int radius;              // not used
    public int rotation;            // tag rotation, according to sensor orientation
    public int rotation_calibrated; // tag rotation, according to relation between sensor orientation and geometric calibration
    public int subtype;             // not used
    public bool tap;                // True if tag was tapped. False otherwise.
    public int tilt_x;              // Tag X axis rotation angle, when in joystick mode. The angle value is calculated from the tag own X axis referenced to a leveled plane (example: a table). Ranges from -90 to 90. When the tag is on top of a leveled plane, X axis rotation value is 0. 
    public int tilt_y;              // Tag Y axis rotation angle, when in joystick mode. The angle value is calculated from the tag own Y axis referenced to a leveled plane (example: a table). Ranges from -90 to 90. When the tag is on top of a leveled plane, Y axis rotation value is 0. 
    public int tilt_z;              // Tag Z axis rotation angle, when in joystick mode. The angle value is calculated from the tag own Z axis referenced to a leveled plane (example: a table). Ranges from -90 to 90. When the tag is on top of a leveled plane, Z axis rotation value is 90. 
    public int type;                // None = 0, Pointer/touch = 1, (passive) 3-point tag = 2, (passive) 4-point tag = 3, (passive) 3-point tag+ = 4, (passive) 4-point tag+ = 5, Pen = 6, Active Tag = 7 (Default)
    public int width;               // tag width. fized value 200
    public int x;                   // tag position X coordinate, according to sensor orientation. Ranges from 0 and 8200. If translateTagCoordsToScreen property in DPXActiveOREngine is true, the coordinate will be translated to current Screen resolution X coordinate.
    public int x_calibrated;        // tag position X coordinate, according to relation between sensor orientation and geometric calibration. Ranges from 0 and 8200. If translateTagCoordsToScreen property in DPXActiveOREngine is true, the coordinate will be translated to current Screen resolution X coordinate.
    public int y;                   // tag position Y coordinate, according to sensor orientation. Ranges from 0 and 4650. If translateTagCoordsToScreen property in DPXActiveOREngine is true, the coordinate will be translated to current Screen resolution Y coordinate.
    public int y_calibrated;        // tag position Y coordinate, according to relation between sensor orientation and geometric calibration. Ranges from 0 and 4650. If translateTagCoordsToScreen property in DPXActiveOREngine is true, the coordinate will be translated to current Screen resolution Y coordinate.
}
```
