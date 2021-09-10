using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXRegisterItem
    {
        public string key;
        public string value;
    }

    [Serializable]
    public class DPXDevicesResponse
    {
        public DPXDevicesReactions reactions;
        public DPXDevicesReactions notifications;
        public string cli;
        public string error;
    }

    [Serializable]
    public class DPXDevicesReactions
    {
        public DPXDeviceReaction[] list;

        public DPXDevicesReactions()
        {
            list = new DPXDeviceReaction[0];
        }

        ~DPXDevicesReactions()
        {
            Array.Clear(list, 0, list.Length);
        }
    }

    [Serializable]
    public class DPXDeviceReaction
    {
        public int id;
        public string key;
        public DPXDevice[] value;
        public string device;
        public DPXRegisterItem[] register;
        public DPXRegisterItem[] unregister;

        public DPXDeviceReaction()
        {
            register = new DPXRegisterItem[0];
            unregister = new DPXRegisterItem[0];
        }

        ~DPXDeviceReaction()
        {
            Array.Clear(register, 0, register.Length);
            Array.Clear(unregister, 0, unregister.Length);
        }
    }
}
    