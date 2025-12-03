using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class MapGeneratorBridge : MonoBehaviour
{
    private const string LibraryName = "SumatoryCore";

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    private static extern float Sumatory_CoreSum(float a, float b);

    void Start()
    {
        float result = SafeSum(5.5f, 4.5f);
        Debug.Log($"[C++ Bridge] Resultado de la suma: {result}");
    }

    public float SafeSum(float a, float b)
    {
        try
        {
            return Sumatory_CoreSum(a, b);
        }
        catch (Exception e)
        {
            Debug.LogError($"Error llamando a la librería nativa: {e.Message}");
            return 0.0f;
        }
    }
}