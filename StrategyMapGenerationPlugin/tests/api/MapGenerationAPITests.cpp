#include <gtest/gtest.h>

#include "api/MapGenerationAPI.h"
#include "hex/HexTile.h"

TEST(MapGenerationAPITest, GeneratesMapWithAccessibleTilesAndCoords) {
    MapGenMapData map = {};

    const int generated = MapGenGenerateMap(6, 4, 1234, 6, 0.5f, 3, nullptr, nullptr, nullptr, &map);
    ASSERT_EQ(generated, 1);
    ASSERT_NE(map.tiles, nullptr);
    ASSERT_EQ(map.width, 6);
    ASSERT_EQ(map.height, 4);
    ASSERT_EQ(map.tileCount, 24);

    bool foundLand = false;
    bool foundOcean = false;
    for (int i = 0; i < map.tileCount; ++i) {
        const MapGenTileData& tile = map.tiles[i];

        EXPECT_GE(tile.r, 0);
        EXPECT_LT(tile.r, map.height);
        EXPECT_GE(tile.terrain, static_cast<int>(TerrainType::DeepOcean));
        EXPECT_LE(tile.terrain, static_cast<int>(TerrainType::Mountain));
        EXPECT_NE(tile.tectonicPlateId, -1);

        if (tile.isLand == 1) {
            foundLand = true;
        }
        if (tile.terrain == static_cast<int>(TerrainType::DeepOcean)) {
            foundOcean = true;
        }
    }

    EXPECT_TRUE(foundLand);
    EXPECT_TRUE(foundOcean);

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, SameSeedProducesDeterministicOutput) {
    MapGenMapData first = {};
    MapGenMapData second = {};

    ASSERT_EQ(MapGenGenerateMap(5, 5, 777, 4, 0.6f, 2, nullptr, nullptr, nullptr, &first), 1);
    ASSERT_EQ(MapGenGenerateMap(5, 5, 777, 4, 0.6f, 2, nullptr, nullptr, nullptr, &second), 1);
    ASSERT_EQ(first.tileCount, second.tileCount);

    for (int i = 0; i < first.tileCount; ++i) {
        EXPECT_EQ(first.tiles[i].q, second.tiles[i].q);
        EXPECT_EQ(first.tiles[i].r, second.tiles[i].r);
        EXPECT_EQ(first.tiles[i].tectonicPlateId, second.tiles[i].tectonicPlateId);
        EXPECT_EQ(first.tiles[i].isLand, second.tiles[i].isLand);
        EXPECT_FLOAT_EQ(first.tiles[i].height, second.tiles[i].height);
        EXPECT_EQ(first.tiles[i].terrain, second.tiles[i].terrain);
    }

    MapGenFreeMap(&first);
    MapGenFreeMap(&second);
}

TEST(MapGenerationAPITest, InvalidInputIsRejected) {
    MapGenMapData map = {};

    EXPECT_EQ(MapGenGenerateMap(0, 4, 1, 2, 0.5f, 3, nullptr, nullptr, nullptr, &map), 0);
    EXPECT_EQ(map.tiles, nullptr);
    EXPECT_EQ(map.tileCount, 0);
}

TEST(MapGenerationAPITest, FreeResetsTheMapData) {
    MapGenMapData map = {};
    ASSERT_EQ(MapGenGenerateMap(3, 3, 42, 3, 0.5f, 2, nullptr, nullptr, nullptr, &map), 1);

    MapGenFreeMap(&map);

    EXPECT_EQ(map.tiles, nullptr);
    EXPECT_EQ(map.width, 0);
    EXPECT_EQ(map.height, 0);
    EXPECT_EQ(map.tileCount, 0);
}
