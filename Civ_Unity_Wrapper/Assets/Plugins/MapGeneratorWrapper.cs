using System;
using System.Runtime.InteropServices;
using UnityEngine;

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

        public MapGenTileData[] tiles;
        public event Action<MapGenTileData[]> OnMapGenerated;

        private MapGenMapData currentMap;

        void Start()
        {
            GenerateMap();
        }

        void OnDestroy()
        {
            if (currentMap.tiles != IntPtr.Zero)
                MapGenFreeMap(ref currentMap);
        }

        public bool GenerateMap()
        {
            if (currentMap.tiles != IntPtr.Zero)
                MapGenFreeMap(ref currentMap);

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

            OnMapGenerated?.Invoke(tiles);
            return true;
        }

        [ContextMenu("Regenerate Map")]
        public void RegenerateMap()
        {
            GenerateMap();
        }
    }
}
