using System;
using System.Runtime.InteropServices;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Plugins
{
    public class MapGeneratorWrapper : MonoBehaviour
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct MapGenTileData
        {
            public int q;
            public int r;
            public int tectonicPlateId;
            public int isLand;
            public float height;
            public int terrain;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MapGenMapData
        {
            public int width;
            public int height;
            public int tileCount;
            public IntPtr tiles;
        }

        public enum TerrainType
        {
            DeepOcean = 0,
            Water = 1,
            Coast = 2,
            Land = 3,
            Mountain = 4
        }

#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
        private const string DllName = "MapGenCore";
#else
        private const string DllName = "libMapGenCore";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int MapGenGenerateMap(int width, int height, int seed, int plateCount, 
            float landRatio, int noiseOctaves, ref MapGenMapData outMap);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void MapGenFreeMap(ref MapGenMapData mapData);

        [Header ("Grid Settings")]
        [Range(4, 20)] public int width = 8;
        [Range(4, 20)] public int height = 6;
        public int seed = 1234;

        [Header ("Random Config")]
        [Range(2, 15)] public int plateCount = 6;
        [Range(0f, 1f)] public float landRatio = 0.5f;
        [Range(1, 5)] public int noiseOctaves = 3;

        [Header("Gizmo Labels")]
        public bool showTerrain = true;
        public bool showPlateId = true;
        public bool showHeight = true;
        public bool showCoordinates = false;

        private MapGenMapData currentMap;
        public MapGenTileData[] tiles;

        void Start()
        {
            GenerateMap();
        }

        void OnDestroy()
        {
            if (currentMap.tiles != IntPtr.Zero)
            {
                MapGenFreeMap(ref currentMap);
            }
        }

        public bool GenerateMap()
        {
            if (currentMap.tiles != IntPtr.Zero)
            {
                MapGenFreeMap(ref currentMap);
            }

            currentMap = new MapGenMapData();
            int result = MapGenGenerateMap(width, height, seed, plateCount, landRatio, noiseOctaves, ref currentMap);

            if (result == 0)
            {
                tiles = null;
                return false;
            }

            tiles = new MapGenTileData[currentMap.tileCount];
            int structSize = Marshal.SizeOf(typeof(MapGenTileData));

            for (int i = 0; i < currentMap.tileCount; i++)
            {
                IntPtr tilePtr = new IntPtr(currentMap.tiles.ToInt64() + (i * structSize));
                tiles[i] = Marshal.PtrToStructure<MapGenTileData>(tilePtr);
            }

            return true;
        }

        [ContextMenu("Regenerate Map")]
        public void RegenerateMap()
        {
            GenerateMap();
        }

        void OnDrawGizmos()
        {
            if (tiles == null) return;

            float hexSize = 1f;

            foreach (var tile in tiles)
            {
                float x = hexSize * Mathf.Sqrt(3f) * (tile.q + tile.r / 2f);
                float z = -hexSize * (3f / 2f) * tile.r; // Negate Z so (0,0) is at top-left
                Vector3 center = new Vector3(x, 0, z);

                Gizmos.color = GetTerrainColor((TerrainType)tile.terrain);
                DrawHexagon(center, hexSize);

#if UNITY_EDITOR
                string label = "";
                if (showTerrain) label += $"{(TerrainType)tile.terrain}\n";
                if (showPlateId) label += $"Plate: {tile.tectonicPlateId}\n";
                if (showHeight) label += $"H: {tile.height:F2}\n";
                if (showCoordinates) label += $"({tile.q},{tile.r})";
                
                if (!string.IsNullOrEmpty(label))
                {
                    GUIStyle style = new GUIStyle();
                    style.alignment = TextAnchor.MiddleCenter;
                    style.normal.textColor = Color.white;
                    Handles.Label(center + Vector3.up * 0.1f, label.TrimEnd('\n'), style);
                }
#endif
            }
        }

        void DrawHexagon(Vector3 center, float size)
        {
            Vector3[] vertices = new Vector3[7];
            for (int i = 0; i < 6; i++)
            {
                float angle = (60f * i + 30f) * Mathf.Deg2Rad;
                vertices[i] = center + new Vector3(
                    size * Mathf.Cos(angle),
                    0,
                    size * Mathf.Sin(angle)
                );
            }
            vertices[6] = vertices[0];

            for (int i = 0; i < 6; i++)
            {
                Gizmos.DrawLine(vertices[i], vertices[i + 1]);
            }
        }

        Color GetTerrainColor(TerrainType terrain)
        {
            switch (terrain)
            {
                case TerrainType.DeepOcean: return new Color(0.1f, 0.2f, 0.5f);
                case TerrainType.Water: return new Color(0.2f, 0.4f, 0.8f);
                case TerrainType.Coast: return new Color(0.8f, 0.8f, 0.6f);
                case TerrainType.Land: return new Color(0.3f, 0.7f, 0.3f);
                case TerrainType.Mountain: return new Color(0.6f, 0.6f, 0.6f);
                default: return Color.magenta;
            }
        }
    }
}