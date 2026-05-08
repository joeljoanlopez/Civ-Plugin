#include <gtest/gtest.h>

#include "api/MapGenerationAPI.h"

static const int defaultCount = 7; // Deep Ocean=0, Ocean=1, Tundra=2, Desert=3, Plains=4, Forest=5, Rainforest=6

TEST(MapGenerationAPITest, GeneratesMapWithAccessibleTilesAndCoords) {
    MapGenMapData map = {};

    const int generated = MapGenGenerateMap(6, 4, 1234, 6, 0.5f, 3, nullptr, 0, nullptr, nullptr, &map);
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
        EXPECT_GE(tile.terrain, 0);
        EXPECT_LT(tile.terrain, defaultCount);
        EXPECT_NE(tile.tectonicPlateId, -1);

        if (tile.isLand == 1) foundLand = true;
        if (tile.terrain == 0) foundOcean = true;
    }

    EXPECT_TRUE(foundLand);
    EXPECT_TRUE(foundOcean);

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, SameSeedProducesDeterministicOutput) {
    MapGenMapData first = {};
    MapGenMapData second = {};

    ASSERT_EQ(MapGenGenerateMap(5, 5, 777, 4, 0.6f, 2, nullptr, 0, nullptr, nullptr, &first), 1);
    ASSERT_EQ(MapGenGenerateMap(5, 5, 777, 4, 0.6f, 2, nullptr, 0, nullptr, nullptr, &second), 1);
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

    EXPECT_EQ(MapGenGenerateMap(0, 4, 1, 2, 0.5f, 3, nullptr, 0, nullptr, nullptr, &map), 0);
    EXPECT_EQ(map.tiles, nullptr);
    EXPECT_EQ(map.tileCount, 0);
}

TEST(MapGenerationAPITest, FreeResetsTheMapData) {
    MapGenMapData map = {};
    ASSERT_EQ(MapGenGenerateMap(3, 3, 42, 3, 0.5f, 2, nullptr, 0, nullptr, nullptr, &map), 1);

    MapGenFreeMap(&map);

    EXPECT_EQ(map.tiles, nullptr);
    EXPECT_EQ(map.width, 0);
    EXPECT_EQ(map.height, 0);
    EXPECT_EQ(map.tileCount, 0);
}

TEST(MapGenerationAPITest, CustomTypes_HighFirstThreshold_AllTilesAreFirstType) {
    MapGenMapData map = {};

    MapGenTerrainTypeDefinition types[] = {
        { "Ocean", 999.0f, -0.45f, 1 },
        { "Land",  1e9f,    0.65f, 0 },
    };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, types, 2, nullptr, nullptr, &map), 1);

    for (int i = 0; i < map.tileCount; ++i) {
        EXPECT_EQ(map.tiles[i].terrain, 0);
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomTypes_VeryLowMaxHeights_AllTilesAreLastType) {
    MapGenMapData map = {};

    MapGenTerrainTypeDefinition types[] = {
        { "A", -999.0f, -0.45f, 1 },
        { "B", -998.0f, -0.05f, 1 },
        { "C",  1e9f,    0.65f, 0 },
    };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, types, 3, nullptr, nullptr, &map), 1);

    for (int i = 0; i < map.tileCount; ++i) {
        EXPECT_EQ(map.tiles[i].terrain, 2);
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomTypes_WiderFirstBucket_MoreFirstTypeTilesVsDefaults) {
    MapGenMapData defaultMap = {};
    MapGenMapData customMap  = {};

    MapGenTerrainTypeDefinition customTypes[] = {
        { "Deep Ocean", 0.5f,  -0.45f, 1 },
        { "Water",      0.7f,  -0.05f, 1 },
        { "Coast",      0.85f,  0.3f,  0 },
        { "Land",       0.95f,  0.65f, 0 },
        { "Mountain",   1e9f,   0.65f, 0 },
    };

    ASSERT_EQ(MapGenGenerateMap(10, 10, 1234, 5, 0.5f, 3, nullptr,     0,    nullptr, nullptr, &defaultMap), 1);
    ASSERT_EQ(MapGenGenerateMap(10, 10, 1234, 5, 0.5f, 3, customTypes, 5,    nullptr, nullptr, &customMap),  1);

    int defaultFirst = 0;
    int customFirst  = 0;
    for (int i = 0; i < defaultMap.tileCount; ++i) {
        if (defaultMap.tiles[i].terrain == 0) ++defaultFirst;
        if (customMap.tiles[i].terrain  == 0) ++customFirst;
    }

    EXPECT_GT(customFirst, defaultFirst);

    MapGenFreeMap(&defaultMap);
    MapGenFreeMap(&customMap);
}

TEST(MapGenerationAPITest, NoiseStrengthZero_TileHeightsAreWithinBaseHeightRange) {
    MapGenMapData map = {};

    TerrainNoiseSettings noNoise = { 0.1f, 1.0f, 2.0f, 0.5f, 2.0f, 0.0f };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, nullptr, 0, &noNoise, nullptr, &map), 1);

    // Default terrain types: water range [-0.45, -0.05], land range [0.3, 0.65]
    for (int i = 0; i < map.tileCount; ++i) {
        if (map.tiles[i].isLand) {
            EXPECT_GE(map.tiles[i].height, 0.3f - 1e-5f);
            EXPECT_LE(map.tiles[i].height, 0.65f + 1e-5f);
        } else {
            EXPECT_GE(map.tiles[i].height, -0.45f - 1e-5f);
            EXPECT_LE(map.tiles[i].height, -0.05f + 1e-5f);
        }
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomBaseHeights_ShiftsTileHeightsVsDefaults) {
    MapGenMapData defaultMap = {};
    MapGenMapData customMap  = {};

    TerrainNoiseSettings noNoise = { 0.1f, 1.0f, 2.0f, 0.5f, 2.0f, 0.0f };

    MapGenTerrainTypeDefinition customTypes[] = {
        { "Deep Ocean", 0.0f, -0.8f, 1 },
        { "Water",      0.2f, -0.2f, 1 },
        { "Coast",      0.4f,  0.5f, 0 },
        { "Land",       0.6f,  0.9f, 0 },
        { "Mountain",   1e9f,  0.9f, 0 },
    };

    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, nullptr,     0,    &noNoise, nullptr, &defaultMap), 1);
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, customTypes, 5,    &noNoise, nullptr, &customMap),  1);

    for (int i = 0; i < defaultMap.tileCount; ++i) {
        if (defaultMap.tiles[i].isLand) {
            EXPECT_GE(customMap.tiles[i].height, defaultMap.tiles[i].height - 1e-5f);
        } else {
            EXPECT_LE(customMap.tiles[i].height, defaultMap.tiles[i].height + 1e-5f);
        }
    }

    MapGenFreeMap(&defaultMap);
    MapGenFreeMap(&customMap);
}

TEST(MapGenerationAPITest, CustomParameters_AreDeterministic) {
    MapGenMapData first  = {};
    MapGenMapData second = {};

    MapGenTerrainTypeDefinition types[] = {
        { "Ocean", 0.3f, -0.3f, 1 },
        { "Coast", 0.5f,  0.2f, 0 },
        { "Land",  0.7f,  0.6f, 0 },
        { "Peak",  1e9f,  0.6f, 0 },
    };
    TerrainNoiseSettings noiseSettings = { 0.2f, 0.8f, 1.5f, 0.4f, 2.5f, 0.3f };

    ASSERT_EQ(MapGenGenerateMap(6, 6, 42, 4, 0.5f, 2, types, 4, &noiseSettings, nullptr, &first),  1);
    ASSERT_EQ(MapGenGenerateMap(6, 6, 42, 4, 0.5f, 2, types, 4, &noiseSettings, nullptr, &second), 1);
    ASSERT_EQ(first.tileCount, second.tileCount);

    for (int i = 0; i < first.tileCount; ++i) {
        EXPECT_EQ(first.tiles[i].q,            second.tiles[i].q);
        EXPECT_EQ(first.tiles[i].r,            second.tiles[i].r);
        EXPECT_EQ(first.tiles[i].isLand,       second.tiles[i].isLand);
        EXPECT_FLOAT_EQ(first.tiles[i].height, second.tiles[i].height);
        EXPECT_EQ(first.tiles[i].terrain,      second.tiles[i].terrain);
    }

    MapGenFreeMap(&first);
    MapGenFreeMap(&second);
}
