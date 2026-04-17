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
        public struct MapGenTileData
        {
            public int q;
            public int r;
            public int tectonicPlateId;
            public int isLand;
            public float height;
            public int terrain;
        }

        public struct MapGenMapData
        {
            public int width;
            public int height;
            public int tileCount;
            public IntPtr tiles;
        }

        [System.Serializable]
        [StructLayout(LayoutKind.Sequential)]
        public struct TerrainThresholds
        {
            public float deepOceanMax;
            public float waterMax;
            public float coastMax;
            public float landMax;
        }

        [System.Serializable]
        [StructLayout(LayoutKind.Sequential)]
        public struct TerrainBaseHeights
        {
            public float landBaseHeight;
            public float waterBaseHeight;
        }

        [System.Serializable]
        [StructLayout(LayoutKind.Sequential)]
        public struct TerrainNoiseSettings
        {
            public float noiseScale;
            public float initialAmplitude;
            public float initialFrequency;
            public float amplitudeDecay;
            public float frequencyMultiplier;
            public float noiseStrength;
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
        private static extern int MapGenGenerateMap(
            int width,
            int height,
            int seed,
            int plateCount,
            float landRatio,
            int noiseOctaves,
            ref TerrainThresholds thresholds,
            ref TerrainBaseHeights baseHeights,
            ref TerrainNoiseSettings noiseSettings,
            ref MapGenMapData outMap
        );

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void MapGenFreeMap(ref MapGenMapData mapData);

        [Header("Grid Settings")] [Range(4, 50)]
        public int width = 8;

        [Range(4, 50)] public int height = 6;
        public int seed = 1234;

        [Header("Random Config")] [Range(2, 15)]
        public int plateCount = 6;

        [Range(0f, 1f)] public float landRatio = 0.5f;
        [Range(1, 5)] public int noiseOctaves = 3;

        [Header("Terrain Thresholds")] public TerrainThresholds terrainThresholds = new TerrainThresholds
        {
            deepOceanMax = 0.0f,
            waterMax = 0.2f,
            coastMax = 0.4f,
            landMax = 0.6f,
        };

        [Header("Base Heights")] public TerrainBaseHeights terrainBaseHeights = new TerrainBaseHeights
        {
            landBaseHeight = 0.5f,
            waterBaseHeight = -0.2f,
        };

        [Header("Noise Settings")] public TerrainNoiseSettings terrainNoiseSettings = new TerrainNoiseSettings
        {
            noiseScale = 0.1f,
            initialAmplitude = 1.0f,
            initialFrequency = 2.0f,
            amplitudeDecay = 0.5f,
            frequencyMultiplier = 2.0f,
            noiseStrength = 0.5f,
        };

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
            int result = MapGenGenerateMap(
                width,
                height,
                seed,
                plateCount,
                landRatio,
                noiseOctaves,
                ref terrainThresholds,
                ref terrainBaseHeights,
                ref terrainNoiseSettings,
                ref currentMap
            );

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
            Vector3[] vertices = new Vector3[6];
            for (int i = 0; i < 6; i++)
            {
                float angle = (60f * i + 30f) * Mathf.Deg2Rad;
                vertices[i] = center + new Vector3(
                    size * Mathf.Cos(angle),
                    0,
                    size * Mathf.Sin(angle)
                );
            }

#if UNITY_EDITOR
            Color fillColor = Gizmos.color;
            fillColor.a = 0.7f;
            Handles.color = fillColor;
            for (int i = 0; i < 6; i++)
            {
                int next = (i + 1) % 6;
                Handles.DrawAAConvexPolygon(center, vertices[i], vertices[next]);
            }
#endif
            Gizmos.color = new Color(0, 0, 0, 0.8f);
            for (int i = 0; i < 6; i++)
            {
                int next = (i + 1) % 6;
                Gizmos.DrawLine(vertices[i], vertices[next]);
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