using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace DPXClasses
{
    public enum DPXtagType { None, Touch, Tag3Point, Tag4Point, TagPlus3Point,TagPlus4Point, Pen, ActiveTag }

    [Serializable]
    public class DPXActiveTag
    {
        public int tag_id;
        public int battery_level;
        public bool double_tap;
        public int height;
        public int identifier;
        public bool in_contact;
        public bool in_range;
        public string name;
        public int pressure;
        public int radius;
        public float rotation;
        public float rotation_calibrated;
        public int subtype;
        public bool tap;
        public float tilt_x;
        public float tilt_y;
        public float tilt_z;
        public int type; // None = 0, Pointer/touch = 1, (passive) 3-point tag = 2, (passive) 4-point tag = 3, (passive) 3-point tag+ = 4, (passive) 4-point tag+ = 5, Pen = 6, Active Tag = 7
        public int width;
        public int x;
        public int x_calibrated;
        public int y;
        public int y_calibrated;
    }

    public class DPXActiveTagObj
    {
        public DPXActiveTag tagData;
        public GameObject tagObj;
    }
}
