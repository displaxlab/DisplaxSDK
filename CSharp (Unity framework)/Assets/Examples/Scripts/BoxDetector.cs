using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using DPXClasses;
using DPXUtils;

public class BoxDetector : MonoBehaviour
{

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnTriggerEnter2D(Collider2D collider)
    {
        GameObject gameObj = GameObject.Find("DPXActiveOREngine");
        DPXActiveOREngine dPXActiveOREngine = gameObj.GetComponent<DPXActiveOREngine>();

        ActiveTag currentTag = collider.GetComponent<ActiveTag>();

        dPXActiveOREngine.SetJoystickMode(currentTag.GetTagData().tag_id);

        dPXActiveOREngine.RemoveTagObjects();

        DPXActiveOREngine.onTagUpdated -= ProcessTagJoystickData;
        DPXActiveOREngine.onTagUpdated += ProcessTagJoystickData;

        GameObject gameObj2 = GameObject.Find("TextInitial");
        Text gameText = gameObj2.GetComponent<Text>();
        gameText.enabled = false;
        gameObj2 = GameObject.Find("BoxDetector");
        gameObj2.GetComponent<Renderer>().enabled = false;

        gameObj2 = GameObject.Find("TextPickAndProceed");
        gameText = gameObj2.GetComponent<Text>();
        gameText.enabled = true;

        gameObj2 = GameObject.Find("SpaceShuttle");
        gameObj2.GetComponent<Renderer>().enabled = true;
    }

    private void ProcessTagJoystickData(DPXActiveTag tagData)
    {
        GameObject gameObj2 = GameObject.Find("SpaceShuttlePackage");
        Transform objTransform = gameObj2.GetComponent<Transform>();

        // Rotate the cube by converting the angles into a quaternion.
        // In this example, we don't use the reported z-axis values.
        Quaternion target = Quaternion.Euler(tagData.tilt_x, -tagData.tilt_y, 0);
        if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
        {
            Debug.Log("x: " + tagData.tilt_x + " ; y: " + tagData.tilt_y + " ; z: " + tagData.tilt_z);
        }
        // Dampen towards the target rotation
        objTransform.rotation = Quaternion.Slerp(objTransform.rotation, target, Time.deltaTime * 10); ;
    }

    public void removeDataHandler()
    {
        DPXActiveOREngine.onTagUpdated -= ProcessTagJoystickData;
    }
}
