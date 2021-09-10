using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace DPXClasses
{
    public class DPXActiveTagNotification
    {
        public string device;
        public string key;
        public DPXActiveTagEntities value;
    }

    [Serializable]
    public class DPXActiveTagEntities
    {
        public DPXActiveTag[] entities;
       //public int or-spec;

        public DPXActiveTagEntities()
        {
            entities = new DPXActiveTag[0];
        }

        ~DPXActiveTagEntities()
        {
            Array.Clear(entities, 0, entities.Length);
        }
    }
}
