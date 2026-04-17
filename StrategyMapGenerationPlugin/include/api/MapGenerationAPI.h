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

typedef struct TerrainThresholds {
    float deepOceanMax;
    float waterMax;
    float coastMax;
    float landMax;
} TerrainThresholds;

typedef struct TerrainBaseHeights {
    float landBaseHeight;
    float waterBaseHeight;
} TerrainBaseHeights;

MAPGEN_API TerrainThresholds MapGenGetTerrainThresholds();
MAPGEN_API TerrainBaseHeights MapGenGetTerrainBaseHeights();

MAPGEN_API int MapGenGenerateMap(
    int width,
    int height,
    int seed,
    int plateCount,
    float landRatio,
    int noiseOctaves,
    MapGenMapData* outMap
);

MAPGEN_API void MapGenFreeMap(MapGenMapData* mapData);

#ifdef __cplusplus
}
#endif
