using System;
using UnityEngine;
using DPXUtils;

namespace DPXUtils
{
    static class DPXConstants
    {
        public const bool DPX_DEBUG_INFO = false; // turns on/off verbose debug information (all debug info)

        public const bool DPX_DEBUG_ACTIVETAGS = false; // turns on/off active tags debug information

        public const int DPX_ACTIONS_TTL_MS = 10000; // maximum wait time (milisseconds) between action request and it's reaction

        public const int DPX_ULTRA_RESOLUTION_X = 8200; // Ultra controller X axis internal resolution
        public const int DPX_ULTRA_RESOLUTION_Y = 4650; // Ultra controller Y axis internal resolution
    }
}