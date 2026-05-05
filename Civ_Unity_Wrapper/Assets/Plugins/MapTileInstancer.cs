using System;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Plugins
{
    [RequireComponent(typeof(MapGeneratorWrapper))]
    public class MapTileInstancer : MonoBehaviour
    {
        [Header("Gizmo Labels")]
        public bool showTerrain = true;
        public bool showPlateId = true;
        public bool showHeight = true;
        public bool showCoordinates = false;

        [Header("Tile Spawning")]
        public bool spawn3DObjects = true;
        public List<TerrainMeshEntry> terrainMeshes = new List<TerrainMeshEntry>();
        [Range(0f, 3f)] public float heightScale = 0.5f;
        [Range(0.5f, 1f)] public float tileScale = 0.9f;
        public float hexSize = 1f;

        [Serializable]
        public class TerrainMeshEntry
        {
            public MapGeneratorWrapper.TerrainType terrain;
            public List<GameObject> prefabs;
        }

        private MapGeneratorWrapper wrapper;
        private MapGeneratorWrapper.MapGenTileData[] tiles;
        private readonly List<GameObject> spawnedTiles = new List<GameObject>();

        void Awake()
        {
            wrapper = GetComponent<MapGeneratorWrapper>();
            wrapper.OnMapGenerated += OnMapGenerated;
        }

        void OnDestroy()
        {
            if (wrapper != null)
                wrapper.OnMapGenerated -= OnMapGenerated;
            DestroySpawnedTiles();
        }

        private void OnMapGenerated(MapGeneratorWrapper.MapGenTileData[] generatedTiles)
        {
            tiles = generatedTiles;
            if (spawn3DObjects)
                SpawnTiles();
            else
                DestroySpawnedTiles();
        }

        private void SpawnTiles()
        {
            DestroySpawnedTiles();

            var meshMap = new Dictionary<MapGeneratorWrapper.TerrainType, List<GameObject>>();
            foreach (var entry in terrainMeshes)
                if (entry != null && entry.prefabs != null && entry.prefabs.Count > 0)
                    meshMap[entry.terrain] = entry.prefabs;

            foreach (var tile in tiles)
            {
                var terrainType = (MapGeneratorWrapper.TerrainType)tile.terrain;
                if (!meshMap.TryGetValue(terrainType, out var prefabList))
                    continue;

                GameObject prefab = prefabList[UnityEngine.Random.Range(0, prefabList.Count)];
                if (prefab == null)
                    continue;

                Vector3 position = GetTileWorldPosition(tile);
                position.y += tile.height * heightScale;

                GameObject tileGameObject = Instantiate(prefab, position, Quaternion.identity, transform);
                tileGameObject.transform.localScale = Vector3.one * tileScale;

                spawnedTiles.Add(tileGameObject);
            }
        }

        private void DestroySpawnedTiles()
        {
            foreach (var tile in spawnedTiles)
                if (tile != null) Destroy(tile);
            spawnedTiles.Clear();
        }

        public Vector3 GetTileWorldPosition(MapGeneratorWrapper.MapGenTileData tile)
        {
            float x = hexSize * Mathf.Sqrt(3f) * (tile.q + tile.r / 2f);
            float z = -hexSize * (3f / 2f) * tile.r;
            return transform.position + new Vector3(x, 0f, z);
        }

        void OnDrawGizmos()
        {
            if (tiles == null) return;

            foreach (var tile in tiles)
            {
                Vector3 center = GetTileWorldPosition(tile);

                Gizmos.color = GetTerrainColor((MapGeneratorWrapper.TerrainType)tile.terrain);
                DrawHexagon(center, hexSize);

#if UNITY_EDITOR
                string label = "";
                if (showTerrain) label += $"{(MapGeneratorWrapper.TerrainType)tile.terrain}\n";
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

        private void DrawHexagon(Vector3 center, float size)
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

        private Color GetTerrainColor(MapGeneratorWrapper.TerrainType terrain)
        {
            switch (terrain)
            {
                case MapGeneratorWrapper.TerrainType.DeepOcean: return new Color(0.1f, 0.2f, 0.5f);
                case MapGeneratorWrapper.TerrainType.Water: return new Color(0.2f, 0.4f, 0.8f);
                case MapGeneratorWrapper.TerrainType.Coast: return new Color(0.8f, 0.8f, 0.6f);
                case MapGeneratorWrapper.TerrainType.Land: return new Color(0.3f, 0.7f, 0.3f);
                case MapGeneratorWrapper.TerrainType.Mountain: return new Color(0.6f, 0.6f, 0.6f);
                default: return Color.magenta;
            }
        }
    }
}
