#include<gtest/gtest.h>
#include "HexCoord.h"

TEST(HexCoordTest, CalculateSCoordinate) {
    HexCoord coord(1, 2);

    EXPECT_EQ(coord.GetQ(), 1);
    EXPECT_EQ(coord.GetR(), 2);
    EXPECT_EQ(coord.GetS(), -3);
}

TEST(HexCoordTest, Equality) {
    HexCoord a(1, 1);
    HexCoord b(1, 1);
    HexCoord c(2, 5);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}