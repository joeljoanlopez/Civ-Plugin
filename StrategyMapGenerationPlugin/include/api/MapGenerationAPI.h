#pragma once

#include "core/CoreAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MapGenTileData {
    int q;
    int r;
    int tectonicPlateId;
    int isLand;
    float height;
    int terrain;
} MapGenTileData;

typedef struct MapGenMapData {
    int width;
    int height;
    int tileCount;
    MapGenTileData* tiles;
} MapGenMapData;

typedef struct MapGenTerrainTypeDefinition {
    char name[64];
    float maxHeight;
    float baseHeight;
    int isWater;
} MapGenTerrainTypeDefinition;

typedef struct TerrainNoiseSettings {
    float noiseScale;
    float initialAmplitude;
    float initialFrequency;
    float amplitudeDecay;
    float frequencyMultiplier;
    float noiseStrength;
} TerrainNoiseSettings;

MAPGEN_API int MapGenGetDefaultTerrainTypeCount();
MAPGEN_API void MapGenGetDefaultTerrainTypes(MapGenTerrainTypeDefinition* outTypes);
MAPGEN_API TerrainNoiseSettings MapGenGetTerrainNoiseSettings();

MAPGEN_API int MapGenGenerateMap(
    int width,
    int height,
    int seed,
    int plateCount,
    float landRatio,
    int noiseOctaves,
    const MapGenTerrainTypeDefinition* terrainTypes,
    int terrainTypeCount,
    const TerrainNoiseSettings* noiseSettings,
    MapGenMapData* outMap
);

MAPGEN_API void MapGenFreeMap(MapGenMapData* mapData);

#ifdef __cplusplus
}
#endif
