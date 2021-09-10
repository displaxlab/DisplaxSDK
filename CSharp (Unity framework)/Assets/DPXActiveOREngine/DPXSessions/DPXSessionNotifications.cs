using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using DPXClasses;
using DPXUtils;
using DPXUtils.SimpleJSON;

namespace DPXSessions
{
    public class DPXSessionNotification
    {
        public int id;
        public DPXNotification notification; // notification (json serializable) 
        public bool sent; // true after being sent to host
        public bool remove; // true if marked to be removed
        public bool readyToCallback; // true if host notification arrived for this notification
        public string callbackParams; // params to pass to callbacks
        public Action<string>[] callbacks; // callbacks to call when notification is received

        public DPXSessionNotification()
        {
            callbacks = new Action<string>[0];
        }

        ~DPXSessionNotification()
        {
            Array.Clear(callbacks, 0, callbacks.Length);
        }
    }

    public class DPXSessionNotifications
    {

        private int id;

        public DPXSessionNotification[] notifications;

        private QJsonDocTranslator jsonTranslator;

        public DPXSessionNotifications()
        {
            id = 0;
            notifications = new DPXSessionNotification[0];
            jsonTranslator = new QJsonDocTranslator();
        }

        // add notification to notifications list
        public bool AddNotification(string key, string device, Action<string> callback = null)
        {
            if (key != "")
            {
                // check if notification for same key,devicemcallback combination already exists. If it doesen't exist, create it.
                if (notifications.Length<=0 || !Array.Exists(notifications, e => e.notification.key == key && e.notification.device == device && Array.Exists(e.callbacks, p => p == callback)))
                {
                    DPXNotification newNotification = new DPXNotification();

                    newNotification.key = key;
                    newNotification.device = device;

                    DPXSessionNotification newSessionNotification = new DPXSessionNotification();

                    newSessionNotification.id = ++id;
                    newSessionNotification.notification = newNotification;
                    newSessionNotification.sent = false;
                    newSessionNotification.remove = false;
                    newSessionNotification.readyToCallback = false;
                    newSessionNotification.callbackParams = "";
                    Array.Resize(ref newSessionNotification.callbacks, newSessionNotification.callbacks.Length + 1);
                    newSessionNotification.callbacks[newSessionNotification.callbacks.Length - 1] = callback;

                    Array.Resize(ref this.notifications, this.notifications.Length + 1);
                    notifications[notifications.Length - 1] = newSessionNotification;

                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("new session notification: " + JsonUtility.ToJson(newNotification, true) + " - " + notifications.Length);
                }
                else 
                {
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("notification already exists. Moving on.");
                }

                return true;
            }
            return false;
        }

        // remove notification from notifications list
        public bool RemoveNotification(string key, string device="")
        {
            if (key != "")
            {
                for(int i=0; i< notifications.Length;i++)
                {
                    if(notifications[i].notification.key==key && notifications[i].notification.device == device)
                    {
                        notifications[i].sent = false;
                        notifications[i].remove = true;
                    }
                }
                return true;
            }
            return false;
        }

        // process notifications received in response data
        public bool ProcessNotificationsFromResponseData(string jsondata)
        {
            var baseData = JSON.Parse(jsondata);

            if (baseData != null && baseData != "")
            {
                for (int i = 0; i < baseData["list"].Count; i++)
                {
                    // if it is a devices_all reaction, treat it differently, as the data structure is different
                    if (baseData["list"][i]["key"] == "devices_list")
                    {
                        var myObject = JsonUtility.FromJson<DPXDeviceReaction>(baseData["list"][i].ToString());
                        DPXSessionNotification obj = Array.Find(notifications, e => e.notification.key == "devices_list"); // as devices_list has no id returned
                                                                                                                           // if obj was found, execute callback (if defined) afterwards (this is due to a Unity limitation on executing calls from objects on different threads)
                        if (obj != null)
                        {
                            obj.readyToCallback = true;
                            obj.callbackParams = JsonUtility.ToJson(myObject);
                        }
                    }
                    else
                    {
                        if (baseData["list"][i]["key"] == "touches_active")
                        {
                            var myObject = JsonUtility.FromJson<DPXActiveTagNotification>(baseData["list"][i].ToString());
                            DPXSessionNotification obj = Array.Find(notifications, e => e.notification.key == "touches_active" && e.notification.device == myObject.device); // as devices_list has no id returned
                                                                                                                               // if obj was found, execute callback (if defined) afterwards (this is due to a Unity limitation on executing calls from objects on different threads)
                            if (obj != null)
                            {
                                obj.readyToCallback = true;
                                obj.callbackParams = JsonUtility.ToJson(myObject);
                            }
                        }
                        else
                        { // other/general notifications
                            var myBaseObject = JsonUtility.FromJson<DPXReaction>(baseData["list"][i].ToString());
                            DPXSessionNotification obj = Array.Find(notifications, e => e.id == myBaseObject.id);
                            // if obj was found, execute callback (if defined) afterwards (this is due to a Unity limitation on executing calls from objects on different threads)
                            if (obj != null)
                            {
                                obj.readyToCallback = true;
                                obj.callbackParams = JsonUtility.ToJson(myBaseObject);
                            }
                        }
                    }
                }
                return true;
            }
            return false;
        }

        // prepare register and unregister lists to be sent in next request
        public DPXAction GetNotificationsRegUnregLists(int newid)
        {
            DPXAction notificationsList = new DPXAction();
            notificationsList.id = newid;
            notificationsList.key = "notifications";
            for (int i = 0; i < notifications.Length; i++)
            {
                if (!notifications[i].sent)
                {
                    if (!notifications[i].remove)
                    {
                        Array.Resize(ref notificationsList.register, notificationsList.register.Length + 1);
                        notificationsList.register[notificationsList.register.Length - 1] = notifications[i].notification;
                        notifications[i].sent = true;
                    }
                    if (notifications[i].remove)
                    {
                        Array.Resize(ref notificationsList.unregister, notificationsList.unregister.Length + 1);
                        notificationsList.unregister[notificationsList.unregister.Length - 1] = notifications[i].notification;
                        notifications[i].sent = true;
                    }
                }
            }
            return notificationsList;
        }

        // update status and process callbacks of each notification on notifications list
        public void UpdateNotifications()
        {

            // process callbacks (this is due to a Unity limitation on executing calls from objects on different threads)
            List<int> idxs = new List<int>();
            for (int i = 0; i < notifications.Length; i++)
            {
                if (notifications[i].sent)
                {
                    if (!notifications[i].remove && notifications[i].readyToCallback && notifications[i].callbacks.Length>0)
                    {
                        notifications[i].readyToCallback = false;
                        for (int p = 0; p < notifications[i].callbacks.Length; p++)
                        {
                            notifications[i].callbacks[p](notifications[i].callbackParams);
                        }
                    }
                    if (notifications[i].remove)
                    {
                        idxs.Add(notifications[i].id);
                    }
                }
            }

            //remove session notifications marked to be removed
            DPXSessionNotification notification;
            foreach (int idx in idxs)
            {
                notification = Array.Find(notifications, e => e.id == idx);
                if (notification != null)
                {
                    Array.Clear(notification.callbacks, 0, notification.callbacks.Length);
                }
                notifications = notifications.Where(e => e.id != idx).ToArray();
            }
        }
    }
}