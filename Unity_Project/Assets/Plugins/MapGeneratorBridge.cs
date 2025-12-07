using System;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Collections.Generic;

public class MapGeneratorBridge : MonoBehaviour
{
    private const string LibraryName = "MapGenerationPlugin";

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]

    private static extern int LineGenerator_GenerateLine(
        float startX, float startY, float startZ,
        int count,
        float spacing,
        char direction,
        [In, Out] float[] points
        );

    [Header("Line Generation Parameters")]
    public Vector3 startPosition;
    public int count = 10;
    public float spacing = 100.0f;
    public string direction = "x";

    [Header("Spawner options")]
    public GameObject actor;

    void Start()
    {
        List<Vector3> points = GenerateLine(startPosition, count, spacing, direction);
        SpawnAtPoints(points, actor);
    }

    public List<Vector3> GenerateLine(Vector3 startpos, int count, float spacing, string direction)
    {
        if (count <= 0) return new List<Vector3>();
        char dirChar = direction[0];

        float[] pointsArray = new float[count * 3];

        int generatedCount = LineGenerator_GenerateLine(
            startpos.x, startpos.y, startpos.z,
            count,
            spacing,
            dirChar,
            pointsArray
        );

        List<Vector3> points = new List<Vector3>();
        for (int i = 0; i < generatedCount; i++)
        {
            float x = pointsArray[i * 3];
            float y = pointsArray[i * 3 + 1];
            float z = pointsArray[i * 3 + 2];
            points.Add(new Vector3(x, y, z));
        }

        return points;
    }

    private void SpawnAtPoints(List<Vector3> points, GameObject actor)
    {
        foreach (var point in points)
        {
            Instantiate(actor, point, Quaternion.identity);
        }
    }
}