
using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using DPXNetwork;
using DPXClasses;
using DPXSessions;
using DPXUtils;
using DPXUtils.SimpleJSON;

namespace DPXSessions
{
    public class DPXSession
    {
        public delegate void OnConnected();
        public static event OnConnected onConnected;

        public delegate void OnDisconnected();
        public static event OnDisconnected onDisconnected;

        public delegate void OnDevicesUpdated();
        public static event OnDevicesUpdated onDevicesUpdated;

        private bool isConnected = false;

        private string remoteHost;
        private int remotePort;

        private DPXDevice[] devices;

        private DPXSessionActions sessionActions;

        private DPXSessionNotifications sessionNotifications;

        private DPXNetConnection netConnection;

        private QJsonDocTranslator jsonTranslator;

        public DPXSession(string newRemoteHost, int newRemotePort)
        {
            remoteHost = newRemoteHost;
            remotePort = newRemotePort;
        }

        public string GetRemoteHost()
        {
            return netConnection.GetRemoteHost();
        }
        public void SetRemoteHost(string host)
        {
            remoteHost = host;
            netConnection.SetRemoteHost(host);
        }

        public int GetRemotePort()
        {
            return netConnection.GetRemotePort();
        }

        public void SetRemotePort(int port)
        {
            remotePort = port;
            netConnection.SetRemotePort(port);
        }

        public bool AddNotification(string key, string device, Action<string> callback = null)
        {
            return isConnected && sessionNotifications.AddNotification(key, device, callback);
        }

        public bool RemoveNotification(string key, string device)
        {
            return isConnected && sessionNotifications.RemoveNotification(key, device);
        }

        public bool AddAction<T>(string key, T value, string device, Action<string> callback = null)
        {
            return isConnected && sessionActions.AddAction<T>(key, value, device, callback);
        }

        public bool AddActiveTagAction(DPXActiveTagAction action, string device)
        {
            return isConnected && sessionActions.AddActiveTagAction(action, device);
        }

        public DPXDevice GetDeviceFromSerialNumber(string serial_number)
        {
            if (!isConnected)
                return null;
            return Array.Find(devices, e => e.serial_number == serial_number);
        }

        public DPXDevice GetDeviceFromEntityName(string entity)
        {
            DPXDevice device=null;

            if (devices != null && isConnected)
            {
                DPXDeviceView view;
                foreach (DPXDevice item in devices)
                {
                    view = Array.Find(item.views, e => e.key == "device-interface-view");
                    if (view != null)
                    {
                        if (Array.Find(view.value, p => p.name == entity) != null)
                        {
                            device = item;
                            break;
                        }
                    }
                }
            }
            return device;
        }

        public void Start()
        {
            isConnected = false;
            devices = new DPXDevice[0];
            sessionActions = new DPXSessionActions();
            sessionNotifications = new DPXSessionNotifications();
            netConnection = new DPXNetConnection();
            jsonTranslator = new QJsonDocTranslator();

            if (remoteHost!="")
                netConnection.SetRemoteHost(remoteHost);
            if (remotePort >0)
                netConnection.SetRemotePort(remotePort);

            DPXNetConnection.onConnected -= OnNetConnected;
            DPXNetConnection.onDisconnected -= OnNetDisconnected;
            DPXNetConnection.onConnected += OnNetConnected;
            DPXNetConnection.onDisconnected += OnNetDisconnected;

            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("Start session");
            netConnection.Init();
        }


        private void OnNetConnected()
        {
            DPXNetConnection.onNewData -= ProcessNewData;
            DPXNetConnection.onNewData += ProcessNewData;

            sessionActions.AddAction<string>("version", "", "", ProcessVersionData);
            SendRequest();
        }

        private void OnNetDisconnected()
        {
            isConnected = false;
            if (onDisconnected != null)
                onDisconnected();

            // remove all devices from the list and notify all delegates

            foreach(DPXDevice device in devices)
            {
                if (device!=null)
                    sessionActions.removeActionsByDevice(device.serial_number);
            }

            Array.Clear(devices, 0, devices.Length);
            devices = new DPXDevice[0];

            if (onDevicesUpdated != null)
                onDevicesUpdated();
        }

        // start to process received response data
        private void ProcessNewData(byte[] data, int length)
        {

            var myBaseObject = JSON.Parse(System.Text.Encoding.UTF8.GetString(data, 0, length));

            if (myBaseObject!= null && (myBaseObject["error"] == "" || myBaseObject["error"] == null))
            {
                // process reactions

                if (myBaseObject["reactions"].Count > 0)
                    sessionActions.ProcessReactionsFromResponseData(myBaseObject["reactions"].ToString());

                //process notifications
                if(myBaseObject["notifications"].Count>0)
                    sessionNotifications.ProcessNotificationsFromResponseData(myBaseObject["notifications"].ToString());
            }
            else
            {
                if (DPXConstants.DPX_DEBUG_INFO)
                    Debug.Log("Error processing new data. Error: " + myBaseObject["error"]);
            }
        }

        // process connected service version
        private void ProcessVersionData(string data)
        {
            // TODO: store version and additional information
            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("Version: " + data);

            sessionNotifications.AddNotification("devices_list", "", ProcessDevicesNotificationData);
            
            requestDeviceList();

            isConnected = true;
            if (onConnected != null)
                onConnected();
        }

        // request devices list
        private void requestDeviceList()
        {
            sessionActions.AddAction<string>("devices_all", "", "", ProcessDevicesData);

            SendRequest();
        }

        // process device notifications
        private void ProcessDevicesNotificationData(string jsondata)
        {
            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("devices notification: " + jsondata);

            requestDeviceList();
        }

        // process devices list in received response data
        private void ProcessDevicesData(string jsondata)
        {
            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("devices: " + jsondata);

            // update device list

            var myObject = JsonUtility.FromJson<DPXDeviceReaction>(jsondata);

            //first, remove devices in devices list
            Array.Clear(devices, 0, devices.Length);
            devices = new DPXDevice[0];

            // add new devices
            if (myObject != null)
            {
                foreach (DPXDevice deviceitem in myObject.value)
                {
                    if (!Array.Exists(devices, element => element.serial_number == deviceitem.serial_number))
                    {
                        Array.Resize(ref devices, devices.Length + 1);
                        devices[devices.Length - 1] = deviceitem;
                    }
                }
                if (DPXConstants.DPX_DEBUG_INFO)
                    foreach (DPXDevice item in devices)
                    {
                        Debug.Log("Device list item: " + item.product_name + " - " + item.serial_number);
                    }
            }

            if (onDevicesUpdated != null)
                onDevicesUpdated();
        }

        // send actions and reg/unregister notificaions list to connected service
        public void SendRequest()
        {
            DPXRequest request=new DPXRequest();

            // process actions
            if (sessionActions.actions.Length > 0)
            {
                DPXAction[] newactions = sessionActions.GetActionsList();

                request.actions.list = newactions;
            }

            //process notifications previously (register and unregister) and, if not empty, add it to actions list
            if (sessionNotifications.notifications.Length > 0)
            {

                DPXAction notifications = sessionNotifications.GetNotificationsRegUnregLists(sessionActions.getId());
 
                //check if there's something to be registered or unregistered
                if (notifications.register.Length>0 || notifications.unregister.Length>0)
                {
                    Array.Resize(ref request.actions.list, request.actions.list.Length + 1);
                    request.actions.list[request.actions.list.Length - 1] = notifications;
                }
            }

            if (request.actions.list.Length > 0)
            {
                if (DPXConstants.DPX_DEBUG_INFO)
                    Debug.Log("Request actions: " + JsonUtility.ToJson(request, true));

                byte[] msg = jsonTranslator.JsonToBinaryData(JsonUtility.ToJson(request));

                netConnection.SendData(msg);
            }

            if (sessionActions.actionsTag.list.Length>0)
            {
                DPXActiveTagRequest requestTags = new DPXActiveTagRequest();

                requestTags.actions = sessionActions.actionsTag;

                //if (DPXConstants.DPX_DEBUG_INFO)
                    Debug.Log("Request active tag actions: " + JsonUtility.ToJson(requestTags, true));

                byte[] msgTags = jsonTranslator.JsonToBinaryData(JsonUtility.ToJson(requestTags));

                netConnection.SendData(msgTags);

                // clear session active tags actions
                sessionActions.ClearActiveTagActions();
            }
            
        }

        // Update is called once per frame
        public void Update()
        {
            // check/maintain connection state
            if (netConnection !=null)
                netConnection.UpdateConnection();

            // process action callbacks (this is due to a Unity limitation on executing calls from objects on different threads)
            if (sessionActions != null)
                sessionActions.UpdateActions();

            // process notification callbacks (this is due to a Unity limitation on executing calls from objects on different threads)
            if (sessionNotifications != null)
                sessionNotifications.UpdateNotifications();
        }

        public void onOnApplicationQuit()
        {
            // signal TCp thread (in netConnection) to stop
            if (netConnection != null)
                netConnection.mustStop();
        }
    }
}