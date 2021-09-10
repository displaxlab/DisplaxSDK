using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VibrationFrictioner : MonoBehaviour
{
    public int frictionValue = 100;

    private int last_pos_x = 0;
    private int last_pos_y = 0;

    private float targetTime = 0f;

    void OnTriggerStay2D(Collider2D collider)
    {
        targetTime -= Time.deltaTime*1000;
        if (targetTime < 0f)
        {
            ActiveTag tag = collider.GetComponent<ActiveTag>();
            if (tag.GetTagData().x_calibrated != last_pos_x || tag.GetTagData().y_calibrated != last_pos_y)
            {
                last_pos_x = tag.GetTagData().x_calibrated;
                last_pos_y = tag.GetTagData().y_calibrated;
                tag.GetTagOREngine().SendTagVibrationEffect(tag.GetTagData().tag_id, 1, frictionValue, 30, 1, true);
            }
            targetTime = 100f;
        }
    }
}
