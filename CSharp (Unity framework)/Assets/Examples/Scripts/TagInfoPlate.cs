using System;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

public class TagInfoPlate : MonoBehaviour
{
    public Text TagIDTextHolder;
    public Text TagBatteryLevelTextHolder;
    public Text TagRotationTextHolder;

    private bool isInside = false;
    private ActiveTag currentTag;

    // Update is called once per frame
    void Update()
    {
        if (isInside && currentTag != null)
        {
            TagBatteryLevelTextHolder.text = "Battery level: " + currentTag.GetTagData().battery_level.ToString();
            TagRotationTextHolder.text = "Rotation: " + currentTag.GetTagData().rotation_calibrated.ToString();
        }
    }

    void OnTriggerEnter2D(Collider2D collider)
    {
        currentTag = collider.GetComponent<ActiveTag>();

        TagIDTextHolder.text = "Tag ID: " + currentTag.GetTagData().tag_id.ToString();

        isInside = true;
    }

    void OnTriggerExit2D(Collider2D collider)
    {
        TagIDTextHolder.text = "Tag ID: ";
        TagBatteryLevelTextHolder.text = "Battery level: ";
        TagRotationTextHolder.text = "Rotation: ";

        isInside = false;
    }
}
