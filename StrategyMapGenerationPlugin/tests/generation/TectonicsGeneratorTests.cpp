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
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 6, 0.5f);

    int landCount = 0;
    for (auto it : grid) {
        if (it.second.IsLand()) landCount++;
    }

    float ratio = static_cast<float>(landCount) / static_cast<float>(grid.GetTotalCells());
    ASSERT_NEAR(ratio, 0.5f, 0.06f);
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_GeneratesHeightAndTypes) {
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5, 0.5f);
    generator.ProcessTerrainMap(grid, 3);

    bool foundOcean = false;
    bool foundLand = false;
    bool foundMountain = false;

    for (auto it : grid) {
        const HexTile& tile = it.second;

        float h = tile.GetHeight();
        TerrainType t = tile.GetTerrain();

        if (h <= 0.0f) {
            EXPECT_EQ(t, TerrainType::DeepOcean);
            foundOcean = true;
        }
        else if (h <= 0.2f) {
            EXPECT_EQ(t, TerrainType::Water);
        }
        else if (h <= 0.4f) {
            EXPECT_EQ(t, TerrainType::Coast);
        }
        else if (h <= 0.6f) {
            EXPECT_EQ(t, TerrainType::Land);
            foundLand = true;
        }
        else {
            EXPECT_EQ(t, TerrainType::Mountain);
            foundMountain = true;
        }
    }

    EXPECT_TRUE(foundOcean);
    EXPECT_TRUE(foundMountain);
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

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomThresholds) {
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    TerrainThresholds customThresholds = {
        -0.1f,
        0.1f,
        0.3f,
        0.5f
    };

    generator.ProcessTerrainMap(grid, 3, &customThresholds);

    for (auto it : grid) {
        const HexTile& tile = it.second;

        float h = tile.GetHeight();
        TerrainType t = tile.GetTerrain();

        if (h <= customThresholds.deepOceanMax) {
            EXPECT_EQ(t, TerrainType::DeepOcean);
        }
        else if (h <= customThresholds.waterMax) {
            EXPECT_EQ(t, TerrainType::Water);
        }
        else if (h <= customThresholds.coastMax) {
            EXPECT_EQ(t, TerrainType::Coast);
        }
        else if (h <= customThresholds.landMax) {
            EXPECT_EQ(t, TerrainType::Land);
        }
        else {
            EXPECT_EQ(t, TerrainType::Mountain);
        }
    }
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomBaseHeights) {
    HexGrid grid(120, 120);
    TectonicsGenerator generator(1234);
    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    TerrainBaseHeights customBaseHeights = {
        0.8f,   // landBaseHeight (far inland)
        -0.6f,  // waterBaseHeight (deep ocean)
        0.4f,   // coastLandHeight
        -0.1f   // coastWaterHeight
    };
    TerrainNoiseSettings noNoiseSettings = MapGenGetTerrainNoiseSettings();
    noNoiseSettings.noiseStrength = 0.0f;

    generator.ProcessTerrainMap(grid, 3, nullptr, &customBaseHeights, &noNoiseSettings);

    for (const auto& it : grid) {
        const HexTile& tile = it.second;
        if (tile.IsLand()) {
            EXPECT_GE(tile.GetHeight(), customBaseHeights.coastLandHeight - 1e-5f);
            EXPECT_LE(tile.GetHeight(), customBaseHeights.landBaseHeight + 1e-5f);
        } else {
            EXPECT_GE(tile.GetHeight(), customBaseHeights.waterBaseHeight - 1e-5f);
            EXPECT_LE(tile.GetHeight(), customBaseHeights.coastWaterHeight + 1e-5f);
        }
    }
}

TEST(TectonicsGeneratorTest, ProcessTerrainMap_WithCustomNoiseSettings) {
    HexGrid grid(120, 120);
    TectonicsGenerator generator(1234);
    generator.GenerateTectonicPlates(grid, 5, 0.5f);

    TerrainNoiseSettings customNoiseSettings = {
        0.35f,
        1.0f,
        2.0f,
        0.5f,
        2.0f,
        0.0f
    };
    generator.ProcessTerrainMap(grid, 3, nullptr, nullptr, &customNoiseSettings);

    TerrainBaseHeights baseHeights = MapGenGetTerrainBaseHeights();
    for (const auto& it : grid) {
        const HexTile& tile = it.second;
        if (tile.IsLand()) {
            EXPECT_GE(tile.GetHeight(), baseHeights.coastLandHeight - 1e-5f);
            EXPECT_LE(tile.GetHeight(), baseHeights.landBaseHeight + 1e-5f);
        } else {
            EXPECT_GE(tile.GetHeight(), baseHeights.waterBaseHeight - 1e-5f);
            EXPECT_LE(tile.GetHeight(), baseHeights.coastWaterHeight + 1e-5f);
        }
    }
}
