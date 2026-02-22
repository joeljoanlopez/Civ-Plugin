#include <gtest/gtest.h>
#include "HexTile.h"

TEST(HexTileTest, DefaultConstructor) {
    HexTile tile;

    EXPECT_EQ(tile.GetTectonicPlateId(), -1);
    EXPECT_FALSE(tile.IsLand());
    EXPECT_EQ(tile.GetHeight(), 0.0f);
    EXPECT_EQ(tile.GetTerrain(), TerrainType::DeepOcean);
}

TEST(HexTileTest, SetAndGetTectonicPlateId) {
    HexTile tile;
    tile.SetTectonicPlateId(5);

    EXPECT_EQ(tile.GetTectonicPlateId(), 5);
}

TEST(HexTileTest, SetAndGetLand) {
    HexTile tile;
    tile.SetLand(true);

    EXPECT_TRUE(tile.IsLand());

    tile.SetLand(false);
    EXPECT_FALSE(tile.IsLand());
}

TEST(HexTileTest, SetAndGetHeight) {
    HexTile tile;
    tile.SetHeight(1.5f);

    EXPECT_EQ(tile.GetHeight(), 1.5f);
}

TEST(HexTileTest, SetAndGetTerrain) {
    HexTile tile;
    tile.SetTerrain(TerrainType::Mountain);

    EXPECT_EQ(tile.GetTerrain(), TerrainType::Mountain);

    tile.SetTerrain(TerrainType::Water);
    EXPECT_EQ(tile.GetTerrain(), TerrainType::Water);
}
