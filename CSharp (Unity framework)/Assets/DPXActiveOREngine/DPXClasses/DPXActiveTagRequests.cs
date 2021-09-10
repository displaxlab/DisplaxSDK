using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXActiveTagRequest
    {
        public DPXActiveTagActionsList actions;

        public DPXActiveTagRequest()
        {
            actions = new DPXActiveTagActionsList();
        }
    }
}