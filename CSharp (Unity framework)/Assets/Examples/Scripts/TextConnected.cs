using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using DPXSessions;

public class TextConnected : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        DPXSession.onConnected += ConnectedText;
        DPXSession.onDisconnected += DisconnectedText;

        GameObject gameObj = GameObject.Find("DPXActiveOREngine");
        DPXActiveOREngine dPXActiveOREngine = gameObj.GetComponent<DPXActiveOREngine>();

        if (dPXActiveOREngine != null)
            gameObject.GetComponent<Text>().text = (dPXActiveOREngine.isConnected ? "Connected" : "Disconnected");
    }

    void ConnectedText()
    {
        gameObject.GetComponent<Text>().text = "Connected";
    }

    void DisconnectedText()
    {
        gameObject.GetComponent<Text>().text = "Disconnected";
    }
}
