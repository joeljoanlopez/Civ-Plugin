#include <gtest/gtest.h>
#include "HexGrid.h"
#include "HexCoord.h"

TEST(HexGridTest, GridSizeInitialization) {
    HexGrid grid(10, 20);

    EXPECT_EQ(grid.GetWidth(), 10);
    EXPECT_EQ(grid.GetHeight(), 20);

    EXPECT_EQ(grid.GetTotalCells(), 200);
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

    // Bottom-right corner in axial coordinates (2,4)
    HexCoord edgeCoord(2, 4);
    std::vector<HexCoord> neighbors = grid.GetNeighbors(edgeCoord);

    EXPECT_EQ(neighbors.size(), 3);
    EXPECT_EQ(neighbors[0], HexCoord(1,4));
    EXPECT_EQ(neighbors[1], HexCoord(2,3));
    EXPECT_EQ(neighbors[2], HexCoord(3,3));
}

TEST(HexGridTest, IsInBoundsValidCoordinates) {
    HexGrid grid(5, 5);

    // Test various valid coordinates
    EXPECT_TRUE(grid.IsInBounds(HexCoord(0, 0)));  // Top-left
    EXPECT_TRUE(grid.IsInBounds(HexCoord(2, 2)));  // Center
    EXPECT_TRUE(grid.IsInBounds(HexCoord(2, 4)));  // Bottom-right
}

TEST(HexGridTest, IsInBoundsInvalidCoordinates) {
    HexGrid grid(5, 5);

    // Test coordinates that are out of bounds
    EXPECT_FALSE(grid.IsInBounds(HexCoord(-1, 0)));   // Negative q
    EXPECT_FALSE(grid.IsInBounds(HexCoord(0, -1)));   // Negative r
    EXPECT_FALSE(grid.IsInBounds(HexCoord(10, 0)));   // q too large
    EXPECT_FALSE(grid.IsInBounds(HexCoord(0, 10)));   // r too large
    EXPECT_FALSE(grid.IsInBounds(HexCoord(5, 5)));    // Both too large
}

TEST(HexGridTest, IsInBoundsEdgeCases) {
    HexGrid grid(3, 3);

    // Test edge coordinates that should be valid
    EXPECT_TRUE(grid.IsInBounds(HexCoord(0, 0)));
    EXPECT_TRUE(grid.IsInBounds(HexCoord(2, 0)));
    EXPECT_TRUE(grid.IsInBounds(HexCoord(1, 2)));

    // Test just outside the bounds
    EXPECT_FALSE(grid.IsInBounds(HexCoord(3, 0)));
    EXPECT_FALSE(grid.IsInBounds(HexCoord(2, 2)));  // This maps to offset (3,2) which is out of bounds
}

TEST(HexGridTest, GetCenterEvenDimensions) {
    HexGrid grid(4, 4);

    HexCoord center = grid.GetGridCenter();

    EXPECT_EQ(center.GetQ(), 2);
    EXPECT_EQ(center.GetR(), 2);
}

TEST(HexGridTest, GetCenterOddDimensions) {
    HexGrid grid(5, 5);

    HexCoord center = grid.GetGridCenter();

    EXPECT_EQ(center.GetQ(), 2);
    EXPECT_EQ(center.GetR(), 2);
}

TEST(HexGridTest, SingleCellGrid) {
    HexGrid grid(1, 1);

    EXPECT_EQ(grid.GetTotalCells(), 1);
    EXPECT_EQ(grid.GetGridCenter(), HexCoord(0, 0));

    std::vector<HexCoord> neighbors = grid.GetNeighbors(HexCoord(0, 0));
    EXPECT_EQ(neighbors.size(), 0);
}

TEST(HexGridTest, GridNeighborsOnMiddle) {
    HexGrid grid(5, 5);

    HexCoord centerCoord = grid.GetGridCenter();
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