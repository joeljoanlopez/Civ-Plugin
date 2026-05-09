#include "api/MapGenerationAPI.h"

#include <algorithm>
#include <cstring>
#include <memory>

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
        //  name             maxH    baseH   water  minT   maxT   minM   maxM
        { "Deep Ocean",  0.0f,  -0.45f,  1,    0.0f,  0.0f,  0.0f,  0.0f },
        { "Ocean",       0.2f,  -0.05f,  1,    0.0f,  0.0f,  0.0f,  0.0f },
        { "Tundra",      1e9f,   0.3f,   0,    0.0f,  0.3f,  0.0f,  1.0f },
        { "Desert",      1e9f,   0.65f,  0,    0.4f,  1.0f,  0.0f,  0.4f },
        { "Plains",      1e9f,   0.65f,  0,    0.3f,  0.8f,  0.3f,  0.65f},
        { "Forest",      1e9f,   0.65f,  0,    0.25f, 0.75f, 0.55f, 1.0f },
        { "Rainforest",  1e9f,   0.65f,  0,    0.6f,  1.0f,  0.65f, 1.0f },
    };
    const int defaultTerrainTypeCount = 7;

    MapGenClimateSettings GetDefaultClimateSettingsInternal() {
        return {
            0.5f,   // equatorNormalizedRow
            0.5f,   // elevationTempPenalty
            0.12f,  // temperatureNoiseStrength
            0.15f,  // moistureNoiseStrength
        };
    }

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

int MapGenGetDefaultTerrainTypes(MapGenTerrainTypeDefinition* outTypes, int outCount) {
    if (outTypes == nullptr || outCount <= 0) return 0;
    int copyCount = std::min(outCount, defaultTerrainTypeCount);
    std::memcpy(outTypes, defaultTerrainTypes, copyCount * sizeof(MapGenTerrainTypeDefinition));
    return copyCount;
}

TerrainNoiseSettings MapGenGetTerrainNoiseSettings() {
    return GetDefaultTerrainNoiseSettings();
}

MapGenClimateSettings MapGenGetDefaultClimateSettings() {
    return GetDefaultClimateSettingsInternal();
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
    const MapGenClimateSettings* climateSettings,
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
    MapGenClimateSettings resolvedClimateSettings = GetDefaultClimateSettingsInternal();
    if (climateSettings) {
        resolvedClimateSettings = *climateSettings;
    }

    generator.ProcessTerrainMap(grid, noiseOctaves, resolvedTypes, resolvedTypeCount, &resolvedNoiseSettings, &resolvedClimateSettings);

    const int totalCells = grid.GetTotalCells();
    auto tileBuffer = std::make_unique<MapGenTileData[]>(totalCells);

    int i = 0;
    for (const auto& it : grid) {
        const HexCoord& coord = it.first;
        const HexTile& tile = it.second;

        tileBuffer[i].q = coord.GetQ();
        tileBuffer[i].r = coord.GetR();
        tileBuffer[i].tectonicPlateId = tile.GetTectonicPlateId();
        tileBuffer[i].isLand = static_cast<int>(tile.IsLand());
        tileBuffer[i].height = tile.GetHeight();
        tileBuffer[i].terrain = tile.GetTerrain();
        tileBuffer[i].temperature = tile.GetTemperature();
        tileBuffer[i].moisture = tile.GetMoisture();
        ++i;
    }

    outMap->width = width;
    outMap->height = height;
    outMap->tileCount = totalCells;
    outMap->tiles = tileBuffer.release();

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
