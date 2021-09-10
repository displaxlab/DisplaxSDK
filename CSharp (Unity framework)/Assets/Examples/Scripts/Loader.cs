using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Loader : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        GameObject gameObj = GameObject.Find("DPXActiveOREngine");
        DPXActiveOREngine dPXActiveOREngine = gameObj.GetComponent<DPXActiveOREngine>();

        // current tag objects should be removed
        if (dPXActiveOREngine != null)
            dPXActiveOREngine.RemoveTagObjects();

        SceneManager.LoadScene(sceneName: "DPXActiveOR_main");
    }
}
