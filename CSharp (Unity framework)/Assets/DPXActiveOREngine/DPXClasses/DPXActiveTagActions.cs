using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXActiveTagActionsList
    {
        public DPXActiveTagActionsItem[] list;

        public DPXActiveTagActionsList()
        {
            list = new DPXActiveTagActionsItem[0];
        }

        ~DPXActiveTagActionsList()
        {
            Array.Clear(list, 0, list.Length);
        }
    }

    [Serializable]
    public class DPXActiveTagActionsItem
    {
        public int id;
        public string key;
        public DPXActiveTagActions value;
        public string type;
        public string device;

        public DPXActiveTagActionsItem()
        {
            value = new DPXActiveTagActions();
        }
    }

    [Serializable]
    public class DPXActiveTagActions
    {
        public DPXActiveTagAction[] actions;
        
        public DPXActiveTagActions()
        {
            actions = new DPXActiveTagAction[0];
        }

        ~DPXActiveTagActions()
        {
            Array.Clear(actions, 0, actions.Length);
        }
    }

    public enum ActiveTagColorEffect { FadeToColor, Blink, ShortPulse, TriangularWave, ToothSawWave, HeartBeat }

    [Serializable]
    public class DPXActiveTagAction
    {
        public int tag_id;
        public string action;
        public string color;
        public int cycles;
        public ActiveTagColorEffect effect;
        public int time;
        public int level;
        public int time_on;
        public int time_off;
        public int enable;
    }
}