using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DPXUtils;

namespace DPXClasses
{
    [Serializable]
    public class DPXRequest
    {
        public DPXActions actions;

        public DPXRequest()
        {
            actions = new DPXActions();
        }
    }
}