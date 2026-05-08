#include <gtest/gtest.h>
#include <list>
#include "hex/HexCoord.h"
#include "hex/HexGrid.h"
#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"


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

    int typeCount = MapGenGetDefaultTerrainTypeCount();
    std::vector<MapGenTerrainTypeDefinition> types(typeCount);
    MapGenGetDefaultTerrainTypes(types.data());

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
    TerrainNoiseSettings noNoiseSettings = MapGenGetTerrainNoiseSettings();
    noNoiseSettings.noiseStrength = 0.0f;

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

    const int typeCount = MapGenGetDefaultTerrainTypeCount();
    std::vector<MapGenTerrainTypeDefinition> types(typeCount);
    MapGenGetDefaultTerrainTypes(types.data());

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
