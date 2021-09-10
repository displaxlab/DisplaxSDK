using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXActions
    {
        public DPXAction[] list;
        
        public DPXActions()
        {
            list = new DPXAction[0];
        }

        ~DPXActions()
        {
            Array.Clear(list, 0, list.Length);
        }
    }

    [Serializable]
    public class DPXAction
    {
        public int id;
        public string key;
        public string value;
        public string type;
        public string device;
        public DPXNotification[] register;
        public DPXNotification[] unregister;
        
        public DPXAction()
        {
            register = new DPXNotification[0];
            unregister = new DPXNotification[0];
        }

        ~DPXAction()
        {
            Array.Clear(register, 0, register.Length);
            Array.Clear(unregister, 0, unregister.Length);
        }
    }
}