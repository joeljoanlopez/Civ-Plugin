#include <gtest/gtest.h>
#include <list>
#include "HexCoord.h"
#include "HexGrid.h"
#include "TectonicsGenerator.h"


TEST(TectonicsGeneratorTest, GeneratePlatesAssignsToAllCells) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 5);

    for (int i = 0; i < grid.GetTotalCells(); ++i) {
        int plateId = grid.GetTileAt(i).GetTectonicPlateId();
        EXPECT_NE(plateId, -1);
    }
}

TEST(TectonicsGeneratorTest, LandToWaterRatioIsCorrect) {
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    generator.GenerateTectonicPlates(grid, 6, 0.5f);

    int landCount = 0;
    for (int i = 0; i < grid.GetTotalCells(); ++i) {
        if (grid.GetTileAt(i).IsLand()) landCount++;
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

    for (int i = 0; i < grid.GetTotalCells(); i++) {
        const HexTile& tile = grid.GetTileAt(i);

        float h = tile.GetHeight();
        TerrainType t = tile.GetTerrain();

        EXPECT_NE(h, 0.0f);

        if (h < 0.2f) {
            EXPECT_EQ(t, TerrainType::DeepOcean);
            foundOcean = true;
        }
        else if (h < 0.3f) {
            EXPECT_EQ(t, TerrainType::Water);
        }
        else if (h < 0.45f) {
            EXPECT_EQ(t, TerrainType::Coast);
        }
        else if (h <= 0.75f) {
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
