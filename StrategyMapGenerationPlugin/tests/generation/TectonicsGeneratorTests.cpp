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

TEST(TectonicsGeneratorTests, ProcessTerrainMap_GeneratesHeightAndTypes) {
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

TEST(TectonicsGeneratorTests, ProcessTerrainMap_WithCustomThresholds) {
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

TEST(TectonicsGeneratorTests, ProcessTerrainMap_WithCustomBaseHeights) {
    HexGrid defaultGrid(120, 120);
    HexGrid customGrid(120, 120);
    TectonicsGenerator defaultGenerator(1234);
    TectonicsGenerator customGenerator(1234);

    defaultGenerator.GenerateTectonicPlates(defaultGrid, 5, 0.5f);
    customGenerator.GenerateTectonicPlates(customGrid, 5, 0.5f);

    defaultGenerator.ProcessTerrainMap(defaultGrid, 3);

    TerrainBaseHeights customBaseHeights = {
        1.5f,
        0.8f
    };
    customGenerator.ProcessTerrainMap(customGrid, 3, nullptr, &customBaseHeights);

    for (int i = 0; i < defaultGrid.GetTotalCells(); ++i) {
        const HexCoord coord = defaultGrid.GetCoordAt(i);
        const HexTile& defaultTile = defaultGrid.GetTileAt(coord);
        const HexTile& customTile = customGrid.GetTileAt(coord);

        EXPECT_EQ(defaultTile.IsLand(), customTile.IsLand());
        EXPECT_NEAR(customTile.GetHeight() - defaultTile.GetHeight(), 1.0f, 1e-5f);
    }
}
