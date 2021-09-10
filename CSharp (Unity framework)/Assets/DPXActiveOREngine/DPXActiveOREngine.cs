using System;
using System.Linq;
using System.Collections.Generic;
using UnityEngine;
using DPXClasses;
using DPXSessions;
using DPXUtils;

public class DPXActiveOREngine : MonoBehaviour
{

    public delegate void OnConnected();
    public static event OnConnected onConnected;

    public delegate void OnDisconnected();
    public static event OnDisconnected onDisconnected;

    public delegate void OnTagUpdated(DPXActiveTag tagData);
    public static event OnTagUpdated onTagUpdated;

    private DPXSession dPXSessionObject;

    public string remoteHost = "127.0.0.1";
    public int remotePort = 45001;

    public bool useFirstAvailableController = true;
    public string controllerSerialNumber;

    public bool translateTagCoordsToScreenResolution = true;

    public bool usePlayerWindowResolution = true;
    
    public bool invertXCalibratedCoords = true;
    public bool invertYCalibratedCoords = false;

    public bool onlyValidTagIDs = true;
    public bool ignoreTapWhenTagIsVibrating = true;

    public GameObject tag_prefab;

    private DPXDevice device;

    private DPXActiveTagObj[] activeTags;

    private bool connected = false;
    public bool isConnected { get { return connected; } }

    private bool m_joystickmode = false;
    public bool joystickMode { get { return m_joystickmode; } }

    private float ignoreTapTimeInitial = 0f;
    private float ignoreTapTimeInterval = 0f;

    private bool flushAllTags = false;

    public DPXActiveOREngine()
    {
        activeTags = new DPXActiveTagObj[0];
    }

    ~DPXActiveOREngine()
    {
        for(int i=0; i< activeTags.Length; i++)
        {
            if (activeTags[i].tagObj != null)
                Destroy(activeTags[i].tagObj);
        }
        Array.Clear(activeTags, 0, activeTags.Length);
    }

    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(this.gameObject);

        connected = false;

        dPXSessionObject = new DPXSession(remoteHost, remotePort);
        dPXSessionObject.Start();

        if (dPXSessionObject!=null)
        {
            DPXSession.onDevicesUpdated -= ProcessDevicesNotificationData;
            DPXSession.onDevicesUpdated += ProcessDevicesNotificationData;

            DPXSession.onDisconnected -= Disconnect;
            DPXSession.onDisconnected += Disconnect;

            ProcessDevicesNotificationData();
        }
        else 
        {
            if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                Debug.Log("DPXActiveOREngine must have an associated DPXSession object.");
        }
    }

    private void Disconnect()
    {
        if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
            Debug.Log("ActiveOREngine disconnected.");

        if (onDisconnected != null)
            onDisconnected();

        for (int i = 0; i < activeTags.Length; i++)
        {
            if (activeTags[i].tagObj != null)
                Destroy(activeTags[i].tagObj);
        }
        Array.Clear(activeTags, 0, activeTags.Length);

        dPXSessionObject.RemoveNotification("at1en", device.serial_number);
        dPXSessionObject.RemoveNotification("touches_active", device.serial_number);
        connected = false;
        device = null;
    }

    // This functions should be called prior to scene change
    public void RemoveTagObjects()
    {
        flushAllTags = true; // flags tag object flushing
    }

    // function to set joystick mode
    // In joystick mode, only tag with tag_id will be used
    public bool SetJoystickMode(int tag_id)
    {
        if (connected && Array.Find(activeTags, e => e.tagData.tag_id == tag_id) != null)
        {
            DPXActiveTagAction action = new DPXActiveTagAction();

            action.tag_id = tag_id;
            action.action = "joystick";
            action.enable = 1;

            dPXSessionObject.AddActiveTagAction(action, device.serial_number);
            dPXSessionObject.SendRequest();

            m_joystickmode = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    // function to set normal operation  mode
    // In joystick mode, only tag with tag_id will be used
    public bool SetNormalMode()
    {
        if (connected)
        {

            DPXActiveTagAction action = new DPXActiveTagAction();

            // set normal mode to all active tags
            foreach (DPXActiveTagObj tag in activeTags)
            {

                action.tag_id = tag.tagData.tag_id;
                action.action = "joystick";
                action.enable = 0;

                dPXSessionObject.AddActiveTagAction(action, device.serial_number);
                dPXSessionObject.SendRequest();
            }

            m_joystickmode = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    // function to send color effect to tag
    public bool SendTagColorEffect(int tag_id, Color color, ActiveTagColorEffect effect, int cycles, int time, bool sendImmediate = false)
    {
        if (connected && Array.Find(activeTags, e => e.tagData.tag_id == tag_id) != null)
        {
            DPXActiveTagAction action = new DPXActiveTagAction();

            action.tag_id = tag_id;
            action.action = "color_effect";
            action.color = "#" + ColorUtility.ToHtmlStringRGB(color);
            action.effect = effect;
            action.cycles = cycles;
            action.time = time;

            dPXSessionObject.AddActiveTagAction(action, device.serial_number);
            if (sendImmediate)
                dPXSessionObject.SendRequest();
            return true;
        }
        else
        {
            return false;
        }
    }

    // function to send vibration effect to tag
    public bool SendTagVibrationEffect(int tag_id, int cycles, int level, int time_on, int time_off, bool sendImmediate = false)
    {
        if (connected && Array.Find(activeTags, e => e.tagData.tag_id == tag_id) != null)
        {
            DPXActiveTagAction action = new DPXActiveTagAction();

            action.tag_id = tag_id;
            action.action = "vibration_effect";
            action.cycles = cycles;
            action.level = level;
            action.time_on = time_on;
            action.time_off = time_off;

            action.color = "#000000";
            action.effect = ActiveTagColorEffect.FadeToColor;
            action.time = 0;

            dPXSessionObject.AddActiveTagAction(action, device.serial_number);
            if (sendImmediate)
                dPXSessionObject.SendRequest();

            if (ignoreTapWhenTagIsVibrating)
            {
                ignoreTapTimeInterval = (float)((time_on + time_off) * (cycles)+1000); // milisseconds ; adds 1 seconds to assure vibration stabilization
                ignoreTapTimeInitial = Time.time*1000; // convert to milisseconds
            }
            
            return true;
        }
        else
        {
            return false;
        }
    }

    private void ProcessDevicesNotificationData()
    {
        if (useFirstAvailableController || (controllerSerialNumber != "" && controllerSerialNumber != null))
        {
            if (!useFirstAvailableController)
            {
                device = dPXSessionObject.GetDeviceFromSerialNumber(controllerSerialNumber);
                if (device!=null)
                {
                    if(device.HasEntity("at1appm"))
                    {
                        //request at1appm value to check further for active OR support
                        dPXSessionObject.AddNotification("at1en", controllerSerialNumber, onReceiveValueAt1appm);
                        dPXSessionObject.SendRequest();
                        dPXSessionObject.AddAction<string>("at1appm", null, controllerSerialNumber, onReceiveValueAt1appm);
                        dPXSessionObject.SendRequest();
                        return;
                    }
                    else
                    {
                        if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                            Debug.Log("Controller with serial number "+ controllerSerialNumber + " does not support Active OR.");
                    }
                }
                else
                {
                    if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                        Debug.Log("Controller with serial number "+ controllerSerialNumber + " not found.");
                }
            }
            else // select first device with ActiveOR capabilities
            {
                device = dPXSessionObject.GetDeviceFromEntityName("at1appm");
                if(device!=null)
                {
                    //request at1appm value to check further for active OR support
                    dPXSessionObject.AddNotification("at1en", device.serial_number, onReceiveValueAt1appm);
                    dPXSessionObject.SendRequest();
                    dPXSessionObject.AddAction<string>("at1appm", null, device.serial_number, onReceiveValueAt1appm);
                    dPXSessionObject.SendRequest();
                    return;
                }
                else
                {
                    if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                        Debug.Log("There's no controllers with Active OR support.");
                }
            }
        }
        else
        {
            if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                Debug.Log("DPXActiveOREngine needs a defined controllerSerialNumber or useFirstAvailableController enabled.");
        }
        connected = false; // worst case, make OR system unavailable
        if (onDisconnected != null)
            onDisconnected();
    }

    private void OnReceiveTagsInfo(string jsondata)
    {
        var myObject = JsonUtility.FromJson<DPXActiveTagNotification>(jsondata);

        if (myObject != null && myObject.device == device.serial_number)
        {
            // check for new tags or updates of existing tags
            foreach (DPXActiveTag tag in myObject.value.entities)
            {
                //process ActiveTag type  only
                if (tag.type == (int)DPXtagType.ActiveTag)
                {
                    // check if only valid ID tags should be reported
                    if (tag.tag_id < 65535 || !onlyValidTagIDs)
                    {
                        //translate coords to match screen coordinates
                        // skip when coordinates are "off-screen" (when x or y values are 65535)
                        if (tag.x < 65535 && tag.y < 65535 && tag.x_calibrated < 65535 && tag.y_calibrated < 65535)
                        {
                            // for now, we assume it's an Ultra controller

                            // translate coords?
                            if (translateTagCoordsToScreenResolution)
                            {
                                var widthScreenValue = 0;
                                var heightScreenValue = 0;

                                // windowed coords, or fullscreen/current full screen resolution?
                                if(usePlayerWindowResolution)
                                {
                                    widthScreenValue = Screen.width;
                                    heightScreenValue = Screen.height;
                                }
                                else
                                {
                                    widthScreenValue = Screen.currentResolution.width;
                                    heightScreenValue = Screen.currentResolution.height;
                                }
                                tag.x = (int)(((float)((float)widthScreenValue / (float)DPXConstants.DPX_ULTRA_RESOLUTION_X)) * (float)tag.x);
                                tag.y = (int)(((float)((float)heightScreenValue / (float)DPXConstants.DPX_ULTRA_RESOLUTION_Y)) * (float)tag.y);
                                tag.x_calibrated = ((int)(((float)((float)widthScreenValue / (float)DPXConstants.DPX_ULTRA_RESOLUTION_X)) * (float)tag.x_calibrated));
                                tag.y_calibrated = ((int)(((float)((float)heightScreenValue / (float)DPXConstants.DPX_ULTRA_RESOLUTION_Y)) * (float)tag.y_calibrated));

                                if (invertXCalibratedCoords)
                                    tag.x_calibrated = widthScreenValue - tag.x_calibrated;
                                if (invertYCalibratedCoords)
                                    tag.y_calibrated = heightScreenValue - tag.y_calibrated;
                            }
                            else
                            {
                                if (invertXCalibratedCoords)
                                    tag.x_calibrated = - tag.x_calibrated;
                                if (invertYCalibratedCoords)
                                    tag.y_calibrated = - tag.y_calibrated;
                            }
                        }

                        DPXActiveTagObj obj = (Array.Find(activeTags, e => e.tagData.tag_id == tag.tag_id));
                        if (obj != null) // already in list
                        {
                            if (JsonUtility.ToJson(obj) != JsonUtility.ToJson(tag)) // use json string comparison to check if the tag info has changed
                            {
                                int idx = Array.FindIndex(activeTags, e => e.tagData.tag_id == tag.tag_id);
                                if (idx >= 0)
                                {
                                    // check if should ignore tap information
                                    if (ignoreTapWhenTagIsVibrating)
                                    {
                                        if ((Time.time*1000) - ignoreTapTimeInitial < ignoreTapTimeInterval)
                                        {
                                            // supress tap info
                                            tag.tap = false;
                                        }
                                    }

                                    if (DPXConstants.DPX_DEBUG_INFO)
                                    {
                                        Debug.Log("UPDATED TAG");
                                        Debug.Log("OBJ: " + JsonUtility.ToJson(obj.tagData, true));
                                        Debug.Log("TAG: " + JsonUtility.ToJson(tag, true));
                                    }
                                    activeTags[idx].tagData = tag;

                                    if (onTagUpdated != null)
                                        onTagUpdated(tag);
                                }
                            }

                        }
                        else // new tag
                        {
                            if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
                                Debug.Log("NEW TAG");

                            // check if should ignore tap information
                            if (ignoreTapWhenTagIsVibrating)
                            {
                                if ((Time.time * 1000) - ignoreTapTimeInitial < ignoreTapTimeInterval)
                                {
                                    // supress tap info
                                    tag.tap = false;
                                }
                            }

                            // add tag info record
                            Array.Resize(ref this.activeTags, this.activeTags.Length + 1);
                            activeTags[this.activeTags.Length - 1] = new DPXActiveTagObj();
                            activeTags[this.activeTags.Length - 1].tagData = tag;
                            //if prefab is defined and not in joystick mode, create active tag game object
                            if (tag_prefab != null && !m_joystickmode)
                            {
                                // instantiates new tag (using defined prefab)
                                activeTags[this.activeTags.Length - 1].tagObj = Instantiate(tag_prefab, Camera.main.ScreenToWorldPoint(new Vector3(tag.x_calibrated, tag.y_calibrated, 1)), Quaternion.identity);

                                // pass the tag dataset into the new tag object
                                GameObject tagObj = activeTags[this.activeTags.Length - 1].tagObj;
                                ActiveTag tagScript = tagObj.GetComponent<ActiveTag>();
                                tagScript.SetTagData(this, tag);
                            }

                            if (onTagUpdated != null)
                                onTagUpdated(tag);
                        }
                    }
                }
            }

            // identify expired tags/not in report
            List<int> idxs = new List<int>();
            foreach (DPXActiveTagObj tag in activeTags)
            {
                var obj = (Array.Find(myObject.value.entities, e => e.tag_id == tag.tagData.tag_id));
                if (obj == null || flushAllTags) // not reported, so it should be removed
                    idxs.Add(tag.tagData.tag_id);
            }

            //remove expired tags/not in report
            foreach (int idx in idxs)
            {

                DPXActiveTagObj tagToDelete = (Array.Find(activeTags, e => e.tagData.tag_id == idx));
                // destroy obj, if exists
                if (tagToDelete.tagObj != null)
                    Destroy(tagToDelete.tagObj);

                if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                    Debug.Log("REMOVED TAG");
                activeTags = activeTags.Where(e => e.tagData.tag_id != idx).ToArray();
            }

            //reset flush flag;
            flushAllTags = false;
        }
    }

    private void onReceiveValueAt1appm(string jsondata)
    {
        var myObject = JsonUtility.FromJson<DPXReaction>(jsondata);

        if (myObject!=null)
        {
            if (myObject.key== "at1appm" && myObject.device == device.serial_number && (myObject.value=="1" || myObject.value == "2"))
            {
                if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS) 
                    Debug.Log("We have an Active OR controller.");
                connected = true;

                if (onConnected != null)
                    onConnected();

                dPXSessionObject.AddNotification("touches_active", device.serial_number, OnReceiveTagsInfo);
                dPXSessionObject.SendRequest();
                return;
            }
        }
        connected = false;  // worst case, make OR system unavailable
        if (onDisconnected != null)
            onDisconnected();
    }

    // Update is called once per frame
    void Update()
    {
        dPXSessionObject.Update();
    }

    void onOnApplicationQuit()
    {
        dPXSessionObject.onOnApplicationQuit();
    }
}
