#include <gtest/gtest.h>
#include "HexGrid.h"
#include "HexCoord.h"

TEST(HexGridTest, GridSizeInitialization)
{
    HexGrid grid(10, 20);

    EXPECT_EQ(grid.GetWidth(), 10);
    EXPECT_EQ(grid.GetHeight(), 20);

    EXPECT_EQ(grid.GetTotalCells(), 200);
}

TEST(HexGridTest, GridOffsetToAxialConversion)
{
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