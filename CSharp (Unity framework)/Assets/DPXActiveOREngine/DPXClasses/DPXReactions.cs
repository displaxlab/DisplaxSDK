using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXResponse
    {
        public DPXReactions reactions;
        public DPXReactions notifications;
        public string cli;
        public string error;
    }

    [Serializable]
    public class DPXReactions
    {
        public DPXReaction[] list;

        public DPXReactions()
        {
            list = new DPXReaction[0];
        }

        ~DPXReactions()
        {
            Array.Clear(list, 0, list.Length);
        }
    }

    [Serializable]
    public class DPXReaction
    {
        public int id;
        public string key;
        public string value;
        public string device;
        public DPXRegisterItem[] register;
        public DPXRegisterItem[] unregister;

        public DPXReaction()
        {
            register = new DPXRegisterItem[0];
            unregister = new DPXRegisterItem[0];
        }

        ~DPXReaction()
        {
            Array.Clear(register, 0, register.Length);
            Array.Clear(unregister, 0, unregister.Length);
        }
    }

    [Serializable]
    public class DPXReactionValue
    {
        public string key;
        public string value;
    }
}

    
