using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Plugins
{
    public class MapGeneratorWrapper : MonoBehaviour
    {
        #region Native Plugin Interop
        
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
#elif UNITY_EDITOR_LINUX || UNITY_STANDALONE_LINUX
        private const string DllName = "libMapGenCore";
#elif UNITY_EDITOR_OSX || UNITY_STANDALONE_OSX
        private const string DllName = "libMapGenCore";
#else
        private const string DllName = "MapGenCore";
#endif

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int MapGenGenerateMap(
            int width,
            int height,
            int seed,
            int plateCount,
            float landRatio,
            int noiseOctaves,
            ref MapGenMapData outMap
        );

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void MapGenFreeMap(ref MapGenMapData mapData);

        #endregion

        #region Public Configuration

        [Header("Map Generation Parameters")]
        [Tooltip("Width of the hexagonal grid")]
        [Range(4, 20)]
        public int width = 8;

        [Tooltip("Height of the hexagonal grid")]
        [Range(4, 20)]
        public int height = 6;

        [Tooltip("Random seed for deterministic generation")]
        public int seed = 1234;

        [Tooltip("Number of tectonic plates")]
        [Range(2, 15)]
        public int plateCount = 6;

        [Tooltip("Proportion of land vs ocean (0.0 = all ocean, 1.0 = all land)")]
        [Range(0f, 1f)]
        public float landRatio = 0.5f;

        [Tooltip("Number of Perlin noise octaves (more = more detail)")]
        [Range(1, 5)]
        public int noiseOctaves = 3;

        [Header("Debug Visualization")]
        [Tooltip("Show terrain type in cells")]
        public bool showTerrain = true;

        [Tooltip("Show tectonic plate IDs")]
        public bool showPlateIds = false;

        [Tooltip("Show height values")]
        public bool showHeights = false;

        [Tooltip("Show hex coordinates")]
        public bool showCoordinates = false;

        [Tooltip("Cell size for debug visualization")]
        [Range(10f, 100f)]
        public float cellSize = 40f;

        #endregion

        #region Private State

        private MapGenMapData currentMap;
        private MapGenTileData[] tiles;
        private bool mapGenerated = false;
        private string lastError = "";

        #endregion

        #region Unity Lifecycle

        private void OnDestroy()
        {
            FreeCurrentMap();
        }

        #endregion

        #region Map Generation

        public bool GenerateMap()
        {
            FreeCurrentMap();

            currentMap = new MapGenMapData();
            int result = MapGenGenerateMap(width, height, seed, plateCount, landRatio, noiseOctaves, ref currentMap);

            if (result == 0)
            {
                lastError = "Map generation failed. Check parameters.";
                mapGenerated = false;
                return false;
            }

            tiles = new MapGenTileData[currentMap.tileCount];
            int structSize = Marshal.SizeOf(typeof(MapGenTileData));

            for (int i = 0; i < currentMap.tileCount; i++)
            {
                IntPtr tilePtr = new IntPtr(currentMap.tiles.ToInt64() + (i * structSize));
                tiles[i] = Marshal.PtrToStructure<MapGenTileData>(tilePtr);
            }

            mapGenerated = true;
            lastError = "";
            return true;
        }

        private void FreeCurrentMap()
        {
            if (currentMap.tiles != IntPtr.Zero)
            {
                MapGenFreeMap(ref currentMap);
                currentMap = new MapGenMapData();
            }
            tiles = null;
            mapGenerated = false;
        }

        #endregion

        #region Debug GUI

        private void OnGUI()
        {
            GUILayout.BeginArea(new Rect(10, 10, 300, Screen.height - 20));

            GUILayout.Label("Strategy Map Generator", GUI.skin.box);

            if (GUILayout.Button("Generate Map"))
            {
                GenerateMap();
            }

            if (!string.IsNullOrEmpty(lastError))
            {
                GUI.color = Color.red;
                GUILayout.Label(lastError);
                GUI.color = Color.white;
            }

            if (mapGenerated)
            {
                GUILayout.Label($"Map: {currentMap.width}x{currentMap.height} ({currentMap.tileCount} tiles)");
                
                int landCount = 0;
                int oceanCount = 0;
                
                foreach (var tile in tiles)
                {
                    if (tile.isLand == 1)
                        landCount++;
                    else
                        oceanCount++;
                }

                GUILayout.Label($"Land: {landCount} | Ocean: {oceanCount}");
            }

            GUILayout.EndArea();

            if (mapGenerated)
            {
                DrawMapGrid();
            }
        }

        private void DrawMapGrid()
        {
            float startX = 320;
            float startY = 10;
            float hexWidth = cellSize * Mathf.Sqrt(3f);
            float hexHeight = cellSize * 2f;
            float verticalSpacing = hexHeight * 0.75f;

            for (int i = 0; i < tiles.Length; i++)
            {
                MapGenTileData tile = tiles[i];
                
                float xOffset = tile.q * hexWidth;
                float yOffset = tile.r * verticalSpacing + (tile.q % 2) * (verticalSpacing / 2f);
                
                float x = startX + xOffset;
                float y = startY + yOffset;

                Color tileColor = GetTerrainColor((TerrainType)tile.terrain);
                DrawHexagon(x, y, cellSize, tileColor);

                string labelText = "";
                if (showTerrain)
                {
                    labelText += ((TerrainType)tile.terrain).ToString() + "\n";
                }
                if (showPlateIds)
                {
                    labelText += $"Plate: {tile.tectonicPlateId}\n";
                }
                if (showHeights)
                {
                    labelText += $"H: {tile.height:F2}\n";
                }
                if (showCoordinates)
                {
                    labelText += $"({tile.q},{tile.r})";
                }

                if (!string.IsNullOrEmpty(labelText))
                {
                    GUI.color = Color.white;
                    GUI.skin.label.alignment = TextAnchor.MiddleCenter;
                    GUI.skin.label.fontSize = 9;
                    GUI.Label(new Rect(x - cellSize / 2, y - cellSize / 2, cellSize, cellSize), labelText);
                }
            }
        }

        private void DrawHexagon(float centerX, float centerY, float size, Color color)
        {
            Vector2[] points = new Vector2[6];
            for (int i = 0; i < 6; i++)
            {
                float angle = 60f * i * Mathf.Deg2Rad;
                points[i] = new Vector2(
                    centerX + size * Mathf.Cos(angle),
                    centerY + size * Mathf.Sin(angle)
                );
            }

            Texture2D texture = new Texture2D(1, 1);
            texture.SetPixel(0, 0, color);
            texture.Apply();

            for (int i = 0; i < 6; i++)
            {
                Vector2 start = points[i];
                Vector2 end = points[(i + 1) % 6];
                DrawLine(start, end, Color.black, 2f);
            }

            GUI.color = color;
            Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
            Vector2 max = new Vector2(float.MinValue, float.MinValue);
            
            foreach (var point in points)
            {
                min.x = Mathf.Min(min.x, point.x);
                min.y = Mathf.Min(min.y, point.y);
                max.x = Mathf.Max(max.x, point.x);
                max.y = Mathf.Max(max.y, point.y);
            }

            GUI.DrawTexture(new Rect(min.x, min.y, max.x - min.x, max.y - min.y), texture);
            GUI.color = Color.white;
        }

        private void DrawLine(Vector2 start, Vector2 end, Color color, float thickness)
        {
            Vector2 diff = end - start;
            float angle = Mathf.Atan2(diff.y, diff.x) * Mathf.Rad2Deg;
            float length = diff.magnitude;

            Texture2D texture = new Texture2D(1, 1);
            texture.SetPixel(0, 0, color);
            texture.Apply();

            Matrix4x4 matrix = Matrix4x4.TRS(
                new Vector3(start.x, start.y, 0),
                Quaternion.Euler(0, 0, angle),
                new Vector3(length, thickness, 1)
            );

            GUI.matrix = matrix;
            GUI.DrawTexture(new Rect(0, -thickness / 2, length, thickness), texture);
            GUI.matrix = Matrix4x4.identity;
        }

        private Color GetTerrainColor(TerrainType terrain)
        {
            switch (terrain)
            {
                case TerrainType.DeepOcean:
                    return new Color(0.1f, 0.2f, 0.5f);
                case TerrainType.Water:
                    return new Color(0.2f, 0.4f, 0.8f);
                case TerrainType.Coast:
                    return new Color(0.8f, 0.8f, 0.6f);
                case TerrainType.Land:
                    return new Color(0.3f, 0.7f, 0.3f);
                case TerrainType.Mountain:
                    return new Color(0.6f, 0.6f, 0.6f);
                default:
                    return Color.magenta;
            }
        }

        #endregion

        #region Public API

        public MapGenTileData[] GetTiles()
        {
            return tiles;
        }

        public bool IsMapGenerated()
        {
            return mapGenerated;
        }

        public MapGenTileData? GetTileAt(int q, int r)
        {
            if (!mapGenerated || tiles == null)
                return null;

            foreach (var tile in tiles)
            {
                if (tile.q == q && tile.r == r)
                    return tile;
            }

            return null;
        }

        #endregion
    }
}