using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ButtonHandler : MonoBehaviour
{
    public void OnClickJoystickScreen()
    {
        GameObject gameObject = GameObject.Find("DPXActiveOREngine");
        DPXActiveOREngine dPXActiveOREngine = gameObject.GetComponent<DPXActiveOREngine>();

        dPXActiveOREngine.SetNormalMode();

        // current tag objects should be removed
        if (dPXActiveOREngine != null)
            dPXActiveOREngine.RemoveTagObjects();

        SceneManager.LoadScene(sceneName: "DPXActiveOR_joystick");
    }

    public void OnClickBackMainScreen()
    {
        GameObject gameObject = GameObject.Find("DPXActiveOREngine");
        DPXActiveOREngine dPXActiveOREngine = gameObject.GetComponent<DPXActiveOREngine>();

        gameObject = GameObject.Find("BoxDetector");
        BoxDetector script = gameObject.GetComponent<BoxDetector>();
        script.removeDataHandler();

        dPXActiveOREngine.SetNormalMode();

        // current tag objects should be removed
        if (dPXActiveOREngine != null)
            dPXActiveOREngine.RemoveTagObjects();

        SceneManager.LoadScene(sceneName: "DPXActiveOR_main");
    }
}
