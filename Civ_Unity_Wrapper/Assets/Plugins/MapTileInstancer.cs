using System;
using System.Collections.Generic;
using UnityEngine;

namespace Plugins
{
    [RequireComponent(typeof(MapGeneratorWrapper))]
    public class MapTileInstancer : MonoBehaviour
    {
        [Header("Tile Spawning")]
        public bool spawn3DObjects = true;
        public List<TerrainMeshEntry> terrainMeshes = new List<TerrainMeshEntry>();
        [Range(0f, 3f)] public float heightScale = 0.5f;
        [Range(0.5f, 1f)] public float tileScale = 0.9f;
        public float hexSize = 1f;

        [Serializable]
        public class TerrainMeshEntry
        {
            [Tooltip("Index matching a terrain type in MapGeneratorWrapper.terrainTypes (0-based).")]
            public int terrainIndex;
            public List<GameObject> prefabs;
        }

        private MapGeneratorWrapper wrapper;
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
            if (spawn3DObjects)
                SpawnTiles(generatedTiles);
            else
                DestroySpawnedTiles();
        }

        private void SpawnTiles(MapGeneratorWrapper.MapGenTileData[] tiles)
        {
            DestroySpawnedTiles();

            var meshMap = new Dictionary<int, List<GameObject>>();
            foreach (var entry in terrainMeshes)
                if (entry != null && entry.prefabs != null && entry.prefabs.Count > 0)
                    meshMap[entry.terrainIndex] = entry.prefabs;

            foreach (var tile in tiles)
            {
                if (!meshMap.TryGetValue(tile.terrain, out var prefabList))
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
    }
}
