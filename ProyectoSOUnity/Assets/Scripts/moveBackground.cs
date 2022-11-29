using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class moveBackground : MonoBehaviour
{
    public Renderer fondo;
    // Update is called once per frame
    void Update()
    {
        fondo.material.mainTextureOffset = fondo.material.mainTextureOffset + new Vector2(0.030f, 0) * Time.deltaTime;
    }
}
