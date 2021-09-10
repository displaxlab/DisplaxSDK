using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColorChanger : MonoBehaviour
{

    public Color color;

    // Start is called before the first frame update
    void Start()
    {
        this.GetComponent<SpriteRenderer>().color = color + new Color(0,0,0,1f);
    }

    void OnTriggerEnter2D(Collider2D collider) {
        ActiveTag tag = collider.GetComponent<ActiveTag>();
        tag.SetCurrentColor(this.GetComponent<SpriteRenderer>().color);
        tag.GetTagOREngine().SendTagColorEffect(tag.GetTagData().tag_id, this.GetComponent<SpriteRenderer>().color, tag.GetCurrentColorEffect(), 5, 3000, true);
    }
}
