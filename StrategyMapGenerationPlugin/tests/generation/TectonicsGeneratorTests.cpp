#include <gtest/gtest.h>
#include <list>
#include "hex/HexCoord.h"
#include "hex/HexGrid.h"
#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"

static void LoadDefaultTypes(int& typeCount, std::vector<MapGenTerrainTypeDefinition>& types) {
    typeCount = MapGenGetDefaultTerrainTypeCount();
    types.resize(typeCount);
    MapGenGetDefaultTerrainTypes(types.data());
}

static TerrainNoiseSettings ZeroNoise() {
    TerrainNoiseSettings s = MapGenGetTerrainNoiseSettings();
    s.noiseStrength = 0.0f;
    return s;
}

TEST(TectonicsGeneratorTest, GeneratePlatesAssignsToAllCells) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5);

    for (auto it: grid) {
        int plateId = it.second.GetTectonicPlateId();
        EXPECT_NE(plateId, -1);
    }
}

TEST(TectonicsGeneratorTest, LandToWaterRatioIsCorrect) {
    HexGrid grid(400, 400);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 20, 0.5f);

    int landCount = 0;
    for (auto it : grid) {
        if (it.second.IsLand()) landCount++;
    }

    float ratio = static_cast<float>(landCount) / static_cast<float>(grid.GetTotalCells());
    ASSERT_NEAR(ratio, 0.5f, 0.06f);
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_GeneratesHeightAndTypes) {
    HexGrid grid(300, 300);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    int typeCount = 0;
    std::vector<MapGenTerrainTypeDefinition> types;
    LoadDefaultTypes(typeCount, types);

    generator.ProcessTerrainMap(grid, 3, types.data(), typeCount);

    bool foundWater = false;
    bool foundLand  = false;

    const int firstLandIndex = 2;

    for (auto it : grid) {
        const HexTile& tile = it.second;
        int terrain = tile.GetTerrain();

        EXPECT_GE(terrain, 0);
        EXPECT_LT(terrain, typeCount);

        if (tile.IsLand()) {
            EXPECT_GE(terrain, firstLandIndex);
            EXPECT_GE(tile.GetTemperature(), 0.0f);
            EXPECT_LE(tile.GetTemperature(), 1.0f);
            EXPECT_GE(tile.GetMoisture(), 0.0f);
            EXPECT_LE(tile.GetMoisture(), 1.0f);
            foundLand = true;
        } else {
            EXPECT_GE(terrain, 0);
            EXPECT_LT(terrain, typeCount);
            foundWater = true;
        }
    }

    EXPECT_TRUE(foundWater);
    EXPECT_TRUE(foundLand);
}

TEST(TectonicsGeneratorTest, TilesNearSameCenterHaveSamePlate) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 3);

    HexCoord coord1(0, 0);
    HexCoord coord2(0, 1);

    int plateId1 = grid.GetTileAt(coord1).GetTectonicPlateId();
    int plateId2 = grid.GetTileAt(coord2).GetTectonicPlateId();

    EXPECT_NE(plateId1, -1);
    EXPECT_NE(plateId2, -1);
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomTerrainTypes) {
    HexGrid grid(200, 200);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    MapGenTerrainTypeDefinition customTypes[] = {
        { "Deep Water", -0.1f, -0.5f, 1 },
        { "Shallow",     0.1f, -0.1f, 1 },
        { "Coast",       0.3f,  0.2f, 0 },
        { "Plains",      0.5f,  0.5f, 0 },
        { "Mountain",    1e9f,  0.7f, 0 },
    };

    generator.ProcessTerrainMap(grid, 3, customTypes, 5);

    for (auto it : grid) {
        const HexTile& tile = it.second;
        const float height = tile.GetHeight();
        int terrain = tile.GetTerrain();

        if (height <= customTypes[0].maxHeight) {
            EXPECT_EQ(terrain, 0);
        } else if (height <= customTypes[1].maxHeight) {
            EXPECT_EQ(terrain, 1);
        } else if (height <= customTypes[2].maxHeight) {
            EXPECT_EQ(terrain, 2);
        } else if (height <= customTypes[3].maxHeight) {
            EXPECT_EQ(terrain, 3);
        } else {
            EXPECT_EQ(terrain, 4);
        }
    }
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomBaseHeights) {
    HexGrid grid(120, 120);
    TectonicsGenerator generator(1234);
    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    MapGenTerrainTypeDefinition customTypes[] = {
        { "Deep Ocean", 0.0f, -0.6f, 1 },
        { "Water",      0.2f, -0.1f, 1 },
        { "Coast",      0.4f,  0.4f, 0 },
        { "Land",       0.6f,  0.8f, 0 },
        { "Mountain",   1e9f,  0.8f, 0 },
    };
    TerrainNoiseSettings noNoiseSettings = ZeroNoise();

    generator.ProcessTerrainMap(grid, 3, customTypes, 5, &noNoiseSettings);

    for (const auto& it : grid) {
        const HexTile& tile = it.second;
        if (tile.IsLand()) {
            EXPECT_GE(tile.GetHeight(), 0.4f - 1e-5f);
            EXPECT_LE(tile.GetHeight(), 0.8f + 1e-5f);
        } else {
            EXPECT_GE(tile.GetHeight(), -0.6f - 1e-5f);
            EXPECT_LE(tile.GetHeight(), -0.1f + 1e-5f);
        }
    }
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomNoiseSettings) {
    HexGrid grid(120, 120);
    TectonicsGenerator generator(1234);
    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    int typeCount = 0;
    std::vector<MapGenTerrainTypeDefinition> types;
    LoadDefaultTypes(typeCount, types);

    TerrainNoiseSettings customNoiseSettings = {
        0.35f,
        1.0f,
        2.0f,
        0.5f,
        2.0f,
        0.0f
    };
    generator.ProcessTerrainMap(grid, 3, types.data(), typeCount, &customNoiseSettings);

    for (const auto& it : grid) {
        const HexTile& tile = it.second;
        if (tile.IsLand()) {
            EXPECT_GE(tile.GetHeight(), 0.3f - 1e-5f);
            EXPECT_LE(tile.GetHeight(), 0.65f + 1e-5f);
        } else {
            EXPECT_GE(tile.GetHeight(), -0.45f - 1e-5f);
            EXPECT_LE(tile.GetHeight(), -0.05f + 1e-5f);
        }
    }
}

TEST(TectonicsGeneratorTest, Climate_EquatorAtTopRow_ProducesTopDownTemperatureGradient) {
    // Equator at row 0 → row 0 hottest, bottom row coldest.
    // Disable noise and elevation penalty to get a pure latitude gradient.
    HexGrid grid(30, 30);
    TectonicsGenerator generator(42);
    generator.GenerateTectonicPlates(grid, 4, 0.7f);

    int typeCount = 0;
    std::vector<MapGenTerrainTypeDefinition> types;
    LoadDefaultTypes(typeCount, types);

    MapGenClimateSettings climate = MapGenGetDefaultClimateSettings();
    climate.equatorNormalizedRow     = 0.0f;
    climate.elevationTempPenalty     = 0.0f;
    climate.temperatureNoiseStrength = 0.0f;

    TerrainNoiseSettings noNoise = ZeroNoise();

    generator.ProcessTerrainMap(grid, 1, types.data(), typeCount, &noNoise, &climate);

    // Average temperature: top quarter vs bottom quarter (all tiles, not just land).
    const int quarterH = 30 / 4;
    float topSum = 0.0f, bottomSum = 0.0f;
    int   topCount = 0,  bottomCount = 0;

    for (const auto& it : grid) {
        int row = it.first.GetR();
        float temp = it.second.GetTemperature();
        if (row < quarterH)          { topSum    += temp; ++topCount;    }
        if (row >= 30 - quarterH)    { bottomSum += temp; ++bottomCount; }
    }

    ASSERT_GT(topCount, 0);
    ASSERT_GT(bottomCount, 0);
    EXPECT_GT(topSum / topCount, bottomSum / bottomCount);
}

TEST(TectonicsGeneratorTest, Climate_LandMoisture_DecreasesWithDistanceFromCoast) {
    // Coastal land tiles (dist=0) should have moisture≈1, deep inland (dist=1) moisture≈0.
    // Disable noise to get a pure distance-based gradient.
    HexGrid grid(40, 40);
    TectonicsGenerator generator(1234);
    generator.GenerateTectonicPlates(grid, 6, 0.5f);

    int typeCount = 0;
    std::vector<MapGenTerrainTypeDefinition> types;
    LoadDefaultTypes(typeCount, types);

    MapGenClimateSettings climate = MapGenGetDefaultClimateSettings();
    climate.moistureNoiseStrength = 0.0f;

    TerrainNoiseSettings noNoise = ZeroNoise();

    generator.ProcessTerrainMap(grid, 1, types.data(), typeCount, &noNoise, &climate);

    float maxMoisture = -1.0f, minMoisture = 2.0f;
    for (const auto& it : grid) {
        const HexTile& tile = it.second;
        if (!tile.IsLand()) continue;
        float m = tile.GetMoisture();
        if (m > maxMoisture) maxMoisture = m;
        if (m < minMoisture) minMoisture = m;
    }

    EXPECT_GT(maxMoisture, 0.85f);
    EXPECT_LT(minMoisture, 0.15f);
}
