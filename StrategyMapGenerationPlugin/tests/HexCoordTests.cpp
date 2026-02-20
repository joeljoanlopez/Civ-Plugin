#include<gtest/gtest.h>
#include "HexCoord.h"

TEST(HexCoordTest, DefaultConstructor) {
    HexCoord coord;

    EXPECT_EQ(coord.GetQ(), 0);
    EXPECT_EQ(coord.GetR(), 0);
    EXPECT_EQ(coord.GetS(), 0);
}

TEST(HexCoordTest, CalculateSCoordinate) {
    HexCoord coord(1, 2);

    EXPECT_EQ(coord.GetQ(), 1);
    EXPECT_EQ(coord.GetR(), 2);
    EXPECT_EQ(coord.GetS(), -3);
}

TEST(HexCoordTest, CubeCoordinateConstraint) {
    HexCoord coord(1, 2);

    EXPECT_EQ(coord.GetQ() + coord.GetR() + coord.GetS(), 0);
}

TEST(HexCoordTest, Equality) {
    HexCoord a(1, 1);
    HexCoord b(1, 1);
    HexCoord c(2, 5);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(HexCoordTest, InequalityDifferentQ) {
    HexCoord a(1, 2);
    HexCoord b(2, 2);

    EXPECT_FALSE(a == b);
}

TEST(HexCoordTest, InequalityDifferentR) {
    HexCoord a(1, 2);
    HexCoord b(1, 3);

    EXPECT_FALSE(a == b);
}

TEST(HexCoordTest, GridDistance) {

    HexCoord coord1(0, 0);
    HexCoord coord2(2, 2);

    int distance = coord1.GetDistance(coord2);
    EXPECT_EQ(distance, 4);
}