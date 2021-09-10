using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace DPXClasses
{
    [Serializable]
    public class DPXDeviceEntitySize
    {
        public int height;
        public int width;
    }

    [Serializable]
    public class DPXDeviceEntity
    {
        public string access_mode;
        public string command;
        public string description;
        public string domain;
        public string name;
        public string type;
        public string arg_exp;
        public string[] values;
        public DPXDeviceEntitySize size;

        ~DPXDeviceEntity()
        {
            Array.Clear(values, 0, values.Length);
        }
    }

    [Serializable]
    public class DPXDeviceView
    {
        public string key;
        public string view;
        public DPXDeviceEntity[] value;

        ~DPXDeviceView()
        {
            Array.Clear(value, 0, value.Length);
        }
    }

    [Serializable]
    public class DPXDevice
    {
        public string device_type;
        public int product_id;
        public string product_name;
        public string serial_number;
        public string version;
        public DPXDeviceView[] views;

        ~DPXDevice()
        {
            Array.Clear(views, 0, views.Length);
        }

        public bool HasEntity(string name)
        {
            DPXDeviceView view;
            view = Array.Find(this.views, e => e.key == "device-interface-view");
            if (view != null)
            {
                if (Array.Find(view.value, p => p.name == name) != null)
                    return true;
            }
            return false;
        }
    }
}
