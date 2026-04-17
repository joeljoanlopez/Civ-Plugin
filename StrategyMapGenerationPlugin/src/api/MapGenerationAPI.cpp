#include "api/MapGenerationAPI.h"

#include "generation/TectonicsGenerator.h"
#include "hex/HexGrid.h"

namespace {
    bool IsValidGenerationRequest(
        const int width,
        const int height,
        const int plateCount,
        const float landRatio,
        const int noiseOctaves
    ) {
        return width > 0
            && height > 0
            && plateCount > 0
            && landRatio >= 0.0f
            && landRatio <= 1.0f
            && noiseOctaves > 0;
    }

    TerrainThresholds GetDefaultTerrainThresholds() {
        return {
            0.0f,
            0.2f,
            0.4f,
            0.6f
        };
    }

    TerrainBaseHeights GetDefaultTerrainBaseHeights() {
        return {
            0.5f,
            -0.2f
        };
    }
}


TerrainThresholds MapGenGetTerrainThresholds() {
    return GetDefaultTerrainThresholds();
}

TerrainBaseHeights MapGenGetTerrainBaseHeights() {
    return GetDefaultTerrainBaseHeights();
}


// Pointer is owned by this method's caller
int MapGenGenerateMap(
    const int width,
    const int height,
    const int seed,
    const int plateCount,
    const float landRatio,
    const int noiseOctaves,
    MapGenMapData* outMap
) {
    if (outMap == nullptr) {
        return 0;
    }

    outMap->width = 0;
    outMap->height = 0;
    outMap->tileCount = 0;
    outMap->tiles = nullptr;

    if (!IsValidGenerationRequest(width, height, plateCount, landRatio, noiseOctaves)) {
        return 0;
    }

    HexGrid grid(width, height);
    TectonicsGenerator generator(seed);
    generator.GenerateTectonicPlates(grid, plateCount, landRatio);

    TerrainThresholds thresholds = GetDefaultTerrainThresholds();
    TerrainBaseHeights baseHeights = GetDefaultTerrainBaseHeights();
    generator.ProcessTerrainMap(grid, noiseOctaves, &thresholds, &baseHeights);

    const int totalCells = grid.GetTotalCells();
    MapGenTileData* tileBuffer = new MapGenTileData[totalCells];

    int i = 0;
    for (const auto& it : grid) {
        const HexCoord& coord = it.first;
        const HexTile& tile = it.second;

        tileBuffer[i].q = coord.GetQ();
        tileBuffer[i].r = coord.GetR();
        tileBuffer[i].tectonicPlateId = tile.GetTectonicPlateId();
        tileBuffer[i].isLand = tile.IsLand() ? 1 : 0;
        tileBuffer[i].height = tile.GetHeight();
        tileBuffer[i].terrain = static_cast<int>(tile.GetTerrain());
        ++i;
    }

    outMap->width = width;
    outMap->height = height;
    outMap->tileCount = totalCells;
    outMap->tiles = tileBuffer;

    return 1;
}

// This method takes ownership of the pointer
void MapGenFreeMap(MapGenMapData* mapData) {
    if (mapData == nullptr) {
        return;
    }

    delete[] mapData->tiles;
    mapData->tiles = nullptr;
    mapData->width = 0;
    mapData->height = 0;
    mapData->tileCount = 0;
}
