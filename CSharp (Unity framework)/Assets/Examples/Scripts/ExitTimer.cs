using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class ExitTimer : MonoBehaviour
{
    public float waitTimeBeforeExitValue = 6.0f; // wait time (in seconds) before exit
    private bool isDown = false; // control flag
    private float waitTimeControl = 0;


    // Start is called before the first frame update
    void Start()
    {
        
    }

    public void OnPointerDown()
    {
        isDown = true;
        waitTimeControl = waitTimeBeforeExitValue;
    }

    public void OnPointerUp()
    {
        isDown = false;
        waitTimeControl = 0.0f;
    }
    // Update is called once per frame
    void Update()
    {
        if (isDown)
        {
            waitTimeControl -= Time.deltaTime;
            if(waitTimeControl <= 0.0f)
            {
                Application.Quit();
            }
        }
    }
}
