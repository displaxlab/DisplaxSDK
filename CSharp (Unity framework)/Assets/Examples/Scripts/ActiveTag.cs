using UnityEngine;
using DPXClasses;
using DPXUtils;

public class ActiveTag : MonoBehaviour
{
    // Instantiates prefabs in a circle formation

    public int numberOfObjects = 6;
    public float radius = 65f;

    private DPXActiveTag m_tagData;
    private GameObject m_buttonprefab;
    private GameObject[] m_buttons;

    private GameObject tagObj;
    private ActiveTagColorEffect m_colorEffect;
    private Color m_color;
    private int m_activebutton = -1;
    private DPXActiveOREngine activeOREngine;

    void OnDestroy()
    {
        DPXActiveOREngine.onTagUpdated -= ProcessTagData;

        for (int i = 0; i < numberOfObjects; i++)
        {
            try
            {
                Destroy(m_buttons[i]);
            }
            catch
            { }
        }
    }
    void Start()
    {

        DPXActiveOREngine.onTagUpdated -= ProcessTagData;
        DPXActiveOREngine.onTagUpdated += ProcessTagData;

        if (m_tagData==null)
            m_tagData = new DPXActiveTag();

        m_buttonprefab = Resources.Load("Prefabs/TagMenuButton") as GameObject;
        m_buttons = new GameObject[numberOfObjects];
        m_color = new Color(0f, 1f, 0f);
        for (int i = 0; i < numberOfObjects; i++)
        {
            float angle = i * Mathf.PI * 2 / numberOfObjects;
            float x = Mathf.Cos(angle) * radius;
            float y = Mathf.Sin(angle) * radius;
            Vector3 pos = transform.position + new Vector3(x, y, 0);
            float angleDegrees = angle * Mathf.Rad2Deg;
            Quaternion rot = Quaternion.Euler(0, 0, angleDegrees);
            m_buttons[i] = Instantiate(m_buttonprefab, pos, rot);
            m_buttons[i].transform.parent = transform;
        }
    }

    public DPXActiveOREngine GetTagOREngine()
    {
        return activeOREngine;
    }

    public DPXActiveTag GetTagData()
    {
        return m_tagData;
    }

    public ActiveTagColorEffect GetCurrentColorEffect()
    {
        return m_colorEffect;
    }

    public Color GetCurrentColor()
    {
        return m_color;
    }

    public void SetCurrentColor(Color newColor)
    {
        m_color = newColor;
    }

    public void SetTagData(DPXActiveOREngine sender, DPXActiveTag tagData)
    {
        // update/set OR engine reference;
        activeOREngine = sender;

        // update/set tag data
        m_tagData = tagData;

        if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
        {
            Debug.Log("SET TAG DATA");
        }
    }

    private void ProcessTagData(DPXActiveTag tagData)
    {
        if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
        {
            Debug.Log("JUST ENTERED TAG UPDATE");
        }
        //check if the new data is for this tag
        if (tagData.tag_id == m_tagData.tag_id && activeOREngine != null && !activeOREngine.joystickMode)
        {
            if (DPXConstants.DPX_DEBUG_INFO || DPXConstants.DPX_DEBUG_ACTIVETAGS)
                Debug.Log("ENTERED TAG UPDATE");

            // update all the needed properties and/or behaviours
            // update tag position
            var spriteObj = transform.Find("TagSprite");
            transform.position = Vector3.Slerp(transform.position, Camera.main.ScreenToWorldPoint(new Vector3(tagData.x_calibrated - 30, tagData.y_calibrated + 30, 1)), Time.deltaTime * 20);

            // update Sprite rotation
            if (spriteObj != null)
                spriteObj.rotation = Quaternion.Euler(0, 0, tagData.rotation_calibrated);

            // check angle range (to enable haptics, if necessary)
            int anglerange = 360 / numberOfObjects;
            if (((int)(((m_tagData.rotation_calibrated+anglerange/2)) / anglerange)) != ((int)((tagData.rotation_calibrated + anglerange / 2) / anglerange)))
            {
                if (activeOREngine != null)
                {
                    Vector3 scaleDiff =  new Vector3(5f, 5f, 0);

                    switch (((int)((tagData.rotation_calibrated + anglerange / 2) / anglerange))% numberOfObjects)
                    {
                        case 0: 
                            m_colorEffect = ActiveTagColorEffect.FadeToColor;
                            if (m_activebutton>= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 0;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        case 1:
                            m_colorEffect = ActiveTagColorEffect.Blink;
                            if (m_activebutton >= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 1;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        case 2:
                            m_colorEffect = ActiveTagColorEffect.ShortPulse;
                            if (m_activebutton >= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 2;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        case 3:
                            m_colorEffect = ActiveTagColorEffect.TriangularWave;
                            if (m_activebutton >= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 3;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        case 4:
                            m_colorEffect = ActiveTagColorEffect.ToothSawWave;
                            if (m_activebutton >= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 4;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        case 5:
                            m_colorEffect = ActiveTagColorEffect.HeartBeat;
                            if (m_activebutton >= 0)
                                m_buttons[m_activebutton].transform.localScale -= scaleDiff;
                            m_activebutton = 5;
                            m_buttons[m_activebutton].transform.localScale += scaleDiff;
                            break;
                        default:
                            m_colorEffect = ActiveTagColorEffect.FadeToColor;
                            break;
                    }
                    activeOREngine.SendTagColorEffect(m_tagData.tag_id, m_color, m_colorEffect, 2, 3000);
                    activeOREngine.SendTagVibrationEffect(m_tagData.tag_id, 1, 80, 50, 1, true);
                }
            }

            // if tapped, change tag color
            if (tagData.tap)
            {
                var obj = GetComponentInChildren<SpriteRenderer>();
                if (obj != null)
                    obj.color = new Color(0f, 1f, 0f, 1f);
            }
            else
            {
                var obj = GetComponentInChildren<SpriteRenderer>();
                if ( obj != null)
                    obj.color = new Color(1f, 1f, 1f, 1f);
            }

            // at the end, update internal tagData dataset with the new one
            m_tagData = tagData;
        }
    }
}