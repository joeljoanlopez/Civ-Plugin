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
