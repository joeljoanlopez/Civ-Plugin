#include "api/MapGenerationAPI.h"

#include <cstring>
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

    const MapGenTerrainTypeDefinition defaultTerrainTypes[] = {
        { "Deep Ocean", 0.0f,  -0.45f, 1 },
        { "Water",      0.2f,  -0.05f, 1 },
        { "Coast",      0.4f,   0.3f,  0 },
        { "Land",       0.6f,   0.65f, 0 },
        { "Mountain",   1e9f,   0.65f, 0 },
    };
    const int defaultTerrainTypeCount = 5;

    TerrainNoiseSettings GetDefaultTerrainNoiseSettings() {
        return {
            0.1f,   // noiseScale
            1.0f,   // initialAmplitude
            2.0f,   // initialFrequency
            0.5f,   // amplitudeDecay
            2.0f,   // frequencyMultiplier
            0.5f    // noiseStrength
        };
    }
}


int MapGenGetDefaultTerrainTypeCount() {
    return defaultTerrainTypeCount;
}

void MapGenGetDefaultTerrainTypes(MapGenTerrainTypeDefinition* outTypes) {
    if (outTypes == nullptr) return;
    std::memcpy(outTypes, defaultTerrainTypes, defaultTerrainTypeCount * sizeof(MapGenTerrainTypeDefinition));
}

TerrainNoiseSettings MapGenGetTerrainNoiseSettings() {
    return GetDefaultTerrainNoiseSettings();
}


// Pointer is owned by this method's caller
int MapGenGenerateMap(
    const int width,
    const int height,
    const int seed,
    const int plateCount,
    const float landRatio,
    const int noiseOctaves,
    const MapGenTerrainTypeDefinition* terrainTypes,
    const int terrainTypeCount,
    const TerrainNoiseSettings* noiseSettings,
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

    const MapGenTerrainTypeDefinition* resolvedTypes = defaultTerrainTypes;
    int resolvedTypeCount = defaultTerrainTypeCount;
    if (terrainTypes && terrainTypeCount > 0) {
        resolvedTypes = terrainTypes;
        resolvedTypeCount = terrainTypeCount;
    }

    TerrainNoiseSettings resolvedNoiseSettings = GetDefaultTerrainNoiseSettings();
    if (noiseSettings) {
        resolvedNoiseSettings = *noiseSettings;
    }

    HexGrid grid(width, height);
    TectonicsGenerator generator(seed);
    generator.GenerateTectonicPlates(grid, plateCount, landRatio);
    generator.ProcessTerrainMap(grid, noiseOctaves, resolvedTypes, resolvedTypeCount, &resolvedNoiseSettings);

    const int totalCells = grid.GetTotalCells();
    auto* tileBuffer = new MapGenTileData[totalCells];

    int i = 0;
    for (const auto& it : grid) {
        const HexCoord& coord = it.first;
        const HexTile& tile = it.second;

        tileBuffer[i].q = coord.GetQ();
        tileBuffer[i].r = coord.GetR();
        tileBuffer[i].tectonicPlateId = tile.GetTectonicPlateId();
        tileBuffer[i].isLand = 0;
        if (tile.IsLand()) tileBuffer[i].isLand = 1;
        tileBuffer[i].height = tile.GetHeight();
        tileBuffer[i].terrain = tile.GetTerrain();
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
