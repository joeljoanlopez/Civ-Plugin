using System;
using System.Collections.Generic;
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

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct NativeTerrainTypeDefinition
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string name;
            public float maxHeight;
            public float baseHeight;
            public int isWater;
        }

        [Serializable]
        public class TerrainTypeDefinition
        {
            public string name = "Terrain";
            public float maxHeight = 0f;
            public float baseHeight = 0f;
            public bool isWater = false;
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

#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
        private const string DllName = "MapGenCore";
#else
        private const string DllName = "libMapGenCore";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int MapGenGetDefaultTerrainTypeCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void MapGenGetDefaultTerrainTypes(
            [Out] NativeTerrainTypeDefinition[] outTypes
        );

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int MapGenGenerateMap(
            int width,
            int height,
            int seed,
            int plateCount,
            float landRatio,
            int noiseOctaves,
            [In] NativeTerrainTypeDefinition[] terrainTypes,
            int terrainTypeCount,
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

        [Header("Terrain Types")]
        public List<TerrainTypeDefinition> terrainTypes = new List<TerrainTypeDefinition>
        {
            new TerrainTypeDefinition { name = "Deep Ocean", maxHeight =  0.0f, baseHeight = -0.45f, isWater = true },
            new TerrainTypeDefinition { name = "Water",      maxHeight =  0.2f, baseHeight = -0.05f, isWater = true },
            new TerrainTypeDefinition { name = "Coast",      maxHeight =  0.4f, baseHeight =  0.3f,  isWater = false },
            new TerrainTypeDefinition { name = "Land",       maxHeight =  0.6f, baseHeight =  0.65f, isWater = false },
            new TerrainTypeDefinition { name = "Mountain",   maxHeight =  1e9f, baseHeight =  0.65f, isWater = false },
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

        public string GetTerrainName(int terrainIndex)
        {
            if (terrainTypes != null && terrainIndex >= 0 && terrainIndex < terrainTypes.Count)
                return terrainTypes[terrainIndex].name;
            return terrainIndex.ToString();
        }

        public bool GenerateMap()
        {
            if (currentMap.tiles != IntPtr.Zero)
                MapGenFreeMap(ref currentMap);

            currentMap = new MapGenMapData();

            NativeTerrainTypeDefinition[] nativeTypes = BuildNativeTypes();

            int result = MapGenGenerateMap(
                width,
                height,
                seed,
                plateCount,
                landRatio,
                noiseOctaves,
                nativeTypes,
                nativeTypes.Length,
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

        [ContextMenu("Reset Terrain Types to Defaults")]
        public void ResetTerrainTypesToDefaults()
        {
            int count = MapGenGetDefaultTerrainTypeCount();
            NativeTerrainTypeDefinition[] nativeDefaults = new NativeTerrainTypeDefinition[count];
            MapGenGetDefaultTerrainTypes(nativeDefaults);

            terrainTypes = new List<TerrainTypeDefinition>(count);
            foreach (var n in nativeDefaults)
            {
                terrainTypes.Add(new TerrainTypeDefinition
                {
                    name = n.name,
                    maxHeight = n.maxHeight,
                    baseHeight = n.baseHeight,
                    isWater = n.isWater != 0,
                });
            }
        }

        private NativeTerrainTypeDefinition[] BuildNativeTypes()
        {
            int count = terrainTypes != null ? terrainTypes.Count : 0;
            if (count == 0)
            {
                int defaultCount = MapGenGetDefaultTerrainTypeCount();
                NativeTerrainTypeDefinition[] defaults = new NativeTerrainTypeDefinition[defaultCount];
                MapGenGetDefaultTerrainTypes(defaults);
                return defaults;
            }

            NativeTerrainTypeDefinition[] result = new NativeTerrainTypeDefinition[count];
            for (int i = 0; i < count; i++)
            {
                TerrainTypeDefinition src = terrainTypes[i];
                result[i] = new NativeTerrainTypeDefinition
                {
                    name = src.name ?? string.Empty,
                    maxHeight = src.maxHeight,
                    baseHeight = src.baseHeight,
                    isWater = src.isWater ? 1 : 0,
                };
            }
            return result;
        }
    }
}
