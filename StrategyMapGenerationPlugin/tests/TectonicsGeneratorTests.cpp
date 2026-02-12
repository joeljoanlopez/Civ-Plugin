#include <gtest/gtest.h>
#include <list>
#include "HexCoord.h"
#include "HexGrid.h"
#include "TectonicsGenerator.h"


TEST(TectonicsGeneratorTest, UniqueCenterGeneration) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    std::list<HexCoord> centers = generator.GenerateTectonicCenters(5, grid);

    ASSERT_EQ(centers.size(), 5);
    for (HexCoord center: centers) {
        EXPECT_TRUE(grid.IsInBounds(center));
    }

    std::set<HexCoord> uniqueCenters(centers.begin(), centers.end());
    EXPECT_EQ(uniqueCenters.size(), centers.size());
}

TEST(TectonicsGeneratorTest, HexCoordAssignedToClosestCenter) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    HexCoord center1 = HexCoord(0, 0);
    center1.SetTectonicPlateId(1);
    HexCoord center2 = grid.GetHexCoord(grid.GetTotalCells() - 1);
    center2.SetTectonicPlateId(2);

    std::list<HexCoord> centers{center1, center2};
    grid.AddTectonicCenters(centers);
    grid.FillTectonicPlates();

    ASSERT_EQ(grid.GetTectonicPlateAt(HexCoord(0,1)), grid.AxisToIndex(center1));
    ASSERT_EQ(grid.GetTectonicPlateAt(HexCoord(4,9)), grid.AxisToIndex(center2));
}

TEST(TectonicGeneratorTest, AllCellsAssignedToAPlate) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    std::list<HexCoord> centers = generator.GenerateTectonicCenters(5, grid);
    grid.AddTectonicCenters(centers);
    grid.FillTectonicPlates();

    for (int i = 0; i < grid.GetTotalCells(); ++i) {
        HexCoord coord = grid.GetHexCoord(i);
        int plateId = grid.GetTectonicPlateAt(coord);
        EXPECT_NE(plateId, -1);
    }
}

TEST(TectonicGeneratorTest, CoordHasSameLandStateAsCenter) {
    HexGrid grid(10, 10);
    TectonicsGenerator generator(1234);

    std::list<HexCoord> centers = generator.GenerateTectonicCenters(5, grid);
    grid.AddTectonicCenters(centers);
    grid.FillTectonicPlates();

    HexCoord coord = grid.GetHexCoord(0);
    HexCoord coordCenter = grid.GetHexCoord(coord.GetTectonicPlateId());

    ASSERT_EQ(coord.IsLand(), coordCenter.IsLand());
}

TEST(TectonicGeneratorTest, LandToWaterRatioIsCorrect) {
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    std::list<HexCoord> centers = generator.GenerateTectonicCenters(6, grid);
    grid.AddTectonicCenters(centers);
    grid.FillTectonicPlates();

    int landCount = 0;
    for (int i = 0; i < grid.GetTotalCells(); ++i) {
        if (grid.GetHexCoord(i).IsLand()) landCount++;
    }

    float ratio = static_cast<float>(landCount) / static_cast<float>(grid.GetTotalCells());
    ASSERT_NEAR(ratio, 0.5f, 0.06f);
}

TEST(TectonicsGeneratorTests, ProcessTerrainMap_GeneratesHeightAndTypes) {
    HexGrid grid(1000, 1000);
    TectonicsGenerator generator(1234);

    std::list<HexCoord> centers = generator.GenerateTectonicCenters(5, grid, 0.5f);
    grid.AddTectonicCenters(centers);
    grid.FillTectonicPlates();

    generator.ProcessTerrainMap(grid, 3);

    bool foundOcean = false;
    bool foundLand = false;
    bool foundMountain = false;

    for (int i = 0; i < grid.GetTotalCells(); i++) {
        HexCoord cell = grid.GetHexCoord(i);

        float h = cell.GetHeight();
        TerrainType t = cell.GetTerrain();

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