#include <gtest/gtest.h>
#include "HexGrid.h"
#include "HexCoord.h"

TEST(HexGridTest, GridSizeInitialization) {
    HexGrid grid(10, 20);

    EXPECT_EQ(grid.GetWidth(), 10);
    EXPECT_EQ(grid.GetHeight(), 20);

    EXPECT_EQ(grid.GetTotalCells(), 200);
}

TEST(HexGridTest, GridOffsetToAxialConversion) {
    HexCoord c1 = HexGrid::OffsetToAxis(0, 0);
    EXPECT_EQ(c1.GetQ(), 0);
    EXPECT_EQ(c1.GetR(), 0);

    HexCoord c2 = HexGrid::OffsetToAxis(1, 1);
    EXPECT_EQ(c2.GetQ(), 1);
    EXPECT_EQ(c2.GetR(), 1);

    HexCoord c3 = HexGrid::OffsetToAxis(0, 2);
    EXPECT_EQ(c3.GetQ(), -1);
    EXPECT_EQ(c3.GetR(), 2);
}

TEST(HexGridTest, GridNeighborsfromOrigin) {
    HexGrid grid(5, 5);

    std::vector<HexCoord> neighbors = grid.GetNeighbors(HexCoord(0,0));

    EXPECT_EQ(neighbors.size(), 2);
    EXPECT_EQ(neighbors[0], HexCoord(1,0));
    EXPECT_EQ(neighbors[1], HexCoord(0,1));
}

TEST(HexGridTest, GridNeighborsOnEdge) {
    HexGrid grid(5, 5);

    std::vector<HexCoord> neighbors = grid.GetNeighbors(HexCoord(4,4));

    EXPECT_EQ(neighbors.size(), 2);
    EXPECT_EQ(neighbors[0], HexCoord(3,4));
    EXPECT_EQ(neighbors[1], HexCoord(4,3));
}

TEST(HexGridTest, GetCenterEvenDimensions) {
    HexGrid grid(4, 4);

    HexCoord center = grid.GetCenter();

    EXPECT_EQ(center.GetQ(), 2);
    EXPECT_EQ(center.GetR(), 2);
}

TEST(HexGridTest, GetCenterOddDimensions) {
    HexGrid grid(5, 5);

    HexCoord center = grid.GetCenter();

    EXPECT_EQ(center.GetQ(), 2);
    EXPECT_EQ(center.GetR(), 2);
}

TEST(HexGridTest, SingleCellGrid) {
    HexGrid grid(1, 1);

    EXPECT_EQ(grid.GetTotalCells(), 1);
    EXPECT_EQ(grid.GetCenter(), HexCoord(0, 0));

    std::vector<HexCoord> neighbors = grid.GetNeighbors(HexCoord(0, 0));
    EXPECT_EQ(neighbors.size(), 0);
}

TEST(HexGridTest, GridNeighborsOnMiddle) {
    HexGrid grid(5, 5);

    HexCoord centerCoord = grid.GetCenter();
    EXPECT_EQ(centerCoord, HexCoord(2,2));
    std::vector<HexCoord> neighbors = grid.GetNeighbors(centerCoord);

    EXPECT_EQ(neighbors.size(), 6);
    EXPECT_EQ(neighbors[0], HexCoord(3,2));
    EXPECT_EQ(neighbors[1], HexCoord(2,3));
    EXPECT_EQ(neighbors[2], HexCoord(1,3));
    EXPECT_EQ(neighbors[3], HexCoord(1,2));
    EXPECT_EQ(neighbors[4], HexCoord(2,1));
    EXPECT_EQ(neighbors[5], HexCoord(3,1));
}
