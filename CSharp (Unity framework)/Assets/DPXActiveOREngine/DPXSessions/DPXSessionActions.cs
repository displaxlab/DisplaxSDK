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
    [Serializable]
    public class DPXSessionAction
    {
        public int id; // unique id
        public DPXAction action; // action (json serializable) 
        public long timestart; // timestamp to check, while not sent, if it should be discarded 
        public bool sent; // true after being sent to host
        public bool readyToCallback; // true if host notification arrived for this notification
        public string callbackParams; // params to pass to callbacks
        public Action<string> callback; // callbacks to call when notification is received
    }

    public class DPXSessionActions
    {

        private int id;

        private QJsonDocTranslator jsonTranslator;

        public DPXSessionAction[] actions;
        public DPXActiveTagActionsList actionsTag;

        public DPXSessionActions()
        {
            id = 0;
            actionsTag = new DPXActiveTagActionsList();
            actions = new DPXSessionAction[0];
            jsonTranslator = new QJsonDocTranslator();
        }

        public int getId()
        {
            return ++id;
        }

        public bool AddActiveTagAction(DPXActiveTagAction action, string device)
        {
            if (action != null)
            {
                var obj = Array.Find(actionsTag.list, e => e.device == device);

                if (obj == null)
                {
                    var obj3 = new DPXActiveTagActions();
                    var obj2 = new DPXActiveTagActionsItem();

                    obj2.id = getId();
                    obj2.key = "touches_active";
                    obj2.type = "TouchEntitiesActive";
                    obj2.device = device;
                    obj2.value = obj3;

                    Array.Resize(ref actionsTag.list, actionsTag.list.Length + 1);
                    actionsTag.list[actionsTag.list.Length - 1] = obj2 ;

                    Debug.Log("Added tag action");
                }

                // re-iterate it
                // try-catch to avoid array length bug (array is empty, but Length is 1)?
                
                obj = Array.Find(actionsTag.list, e => e.device == device);

                if (obj != null)
                {
                    Array.Resize(ref obj.value.actions, obj.value.actions.Length + 1);
                    obj.value.actions[obj.value.actions.Length - 1] = action;

                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("new session active tag action: " + JsonUtility.ToJson(action, true));

                    return true;
                }
            }
            return false;
        }

        public bool AddAction<T>(string key, T value, string device, Action<string> callback = null)
        {
            if (key != "")
            {
                string tmpValue = (value!=null?value.ToString():null);

                DPXAction newAction = new DPXAction();

                newAction.id = ++id;
                newAction.key = key;
                newAction.value = tmpValue;
                newAction.device = device;

                DPXSessionAction newSessionAction = new DPXSessionAction();
                newSessionAction.id = id;
                newSessionAction.action = newAction;
                newSessionAction.sent = false;
                newSessionAction.readyToCallback = false;
                newSessionAction.callbackParams = "";
                newSessionAction.timestart = DateTimeOffset.Now.ToUnixTimeMilliseconds();
                newSessionAction.callback = callback;

                Array.Resize(ref this.actions, this.actions.Length + 1);
                actions[actions.Length - 1] = newSessionAction;

                if (DPXConstants.DPX_DEBUG_INFO)
                    Debug.Log("new session action: " + JsonUtility.ToJson(newAction, true)+ " - " + actions.Length);

                return true;
            }
            return false;
        }

        // process reactions received in response data
        public bool ProcessReactionsFromResponseData(string jsondata)
        {
            var myBaseObject = JsonUtility.FromJson<DPXReactions>(jsondata);

            if (myBaseObject != null)
            {
                // process reactions
                for (int i = 0; i < myBaseObject.list.Length; i++)
                {
                    // if it is a devices_all reaction, treat it differently, as the data structure is different
                    if (myBaseObject.list[i].key == "devices_all")
                    {
                        var myObject = JsonUtility.FromJson<DPXDevicesReactions>(jsondata);
                        DPXSessionAction obj = Array.Find(actions, e => e.id == myObject.list[i].id);
                        // if obj was found, execute callback (if defined) afterwards (this is due to a Unity limitation on executing calls from objects on different threads)
                        if (obj != null)
                        {
                            obj.readyToCallback = true;
                            obj.callbackParams = JsonUtility.ToJson(myObject.list[i]);
                        }
                    }
                    else
                    {
                        DPXSessionAction obj = Array.Find(actions, e => e.id == myBaseObject.list[i].id);
                        // if obj was found, execute callback (if defined) afterwards (this is due to a Unity limitation on executing calls from objects on different threads)
                        if (obj != null)
                        {
                            obj.readyToCallback = true;
                            obj.callbackParams = JsonUtility.ToJson(myBaseObject.list[i]);
                        }
                    }
                }
                return true;
            }
            return false;
        }

        // prepare actions list to be sent in next request
        public DPXAction[] GetActionsList()
        {
            DPXAction[] newactions = new DPXAction[0];
            for (int i = 0; i < actions.Length; i++)
            {
                if (!actions[i].sent)
                {
                    Array.Resize(ref newactions, newactions.Length + 1);
                    newactions[newactions.Length - 1] = actions[i].action;
                    actions[i].sent = true;
                }
            }
            return newactions;
        }

        // Update is called once per frame
        public void UpdateActions()
        {

            // process callbacks (this is due to a Unity limitation on executing calls from objects on different threads) and check for timed out actions
            List<int> ids = new List<int>();
            for (int i = 0; i < actions.Length; i++)
            {
                if (actions[i].sent && actions[i].readyToCallback && actions[i].callback != null)
                {
                    actions[i].readyToCallback = false;
                    actions[i].callback(actions[i].callbackParams);
                    ids.Add(actions[i].id);
                }
                else
                {
                    //check if action timed out
                    if (actions[i].timestart + DPXConstants.DPX_ACTIONS_TTL_MS < DateTimeOffset.Now.ToUnixTimeMilliseconds())
                        ids.Add(actions[i].id);
                }
            }

            //remove session actions already processed or timed out

            foreach(int idx in ids)
            {
                actions = actions.Where(e => e.id != idx).ToArray();
            }
        }

        // remove actions destined to a particular device
        public void removeActionsByDevice(string device)
        {
            actions = actions.Where(e => e.action.device != device).ToArray();
        }

       public void  ClearActiveTagActions()
        {
            Array.Resize(ref actionsTag.list, 0);
        }
    }
}