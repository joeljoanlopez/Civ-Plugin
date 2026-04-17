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

TEST(MapGenerationAPITest, CustomThresholds_HighDeepOceanMax_AllTilesAreDeepOcean) {
    MapGenMapData map = {};

    TerrainThresholds thresholds = { 999.0f, 999.0f, 999.0f, 999.0f };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, &thresholds, nullptr, nullptr, &map), 1);

    for (int i = 0; i < map.tileCount; ++i) {
        EXPECT_EQ(map.tiles[i].terrain, static_cast<int>(TerrainType::DeepOcean));
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomThresholds_VeryLowLandMax_AllTilesAreMountain) {
    MapGenMapData map = {};

    TerrainThresholds thresholds = { -999.0f, -998.0f, -997.0f, -996.0f };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, &thresholds, nullptr, nullptr, &map), 1);

    for (int i = 0; i < map.tileCount; ++i) {
        EXPECT_EQ(map.tiles[i].terrain, static_cast<int>(TerrainType::Mountain));
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomThresholds_ChangesTerrainDistributionVsDefaults) {
    MapGenMapData defaultMap = {};
    MapGenMapData customMap  = {};

    // Raising deepOceanMax from 0.0 to 0.5 reclassifies Water/Coast tiles as DeepOcean
    TerrainThresholds shifted = { 0.5f, 0.7f, 0.85f, 0.95f };

    ASSERT_EQ(MapGenGenerateMap(10, 10, 1234, 5, 0.5f, 3, nullptr,  nullptr, nullptr, &defaultMap), 1);
    ASSERT_EQ(MapGenGenerateMap(10, 10, 1234, 5, 0.5f, 3, &shifted, nullptr, nullptr, &customMap),  1);

    int defaultDeepOcean = 0;
    int customDeepOcean  = 0;
    for (int i = 0; i < defaultMap.tileCount; ++i) {
        if (defaultMap.tiles[i].terrain == static_cast<int>(TerrainType::DeepOcean)) ++defaultDeepOcean;
        if (customMap.tiles[i].terrain  == static_cast<int>(TerrainType::DeepOcean)) ++customDeepOcean;
    }

    // Wider DeepOcean bucket must absorb tiles that were Water/Coast under default thresholds
    EXPECT_GT(customDeepOcean, defaultDeepOcean);

    MapGenFreeMap(&defaultMap);
    MapGenFreeMap(&customMap);
}

TEST(MapGenerationAPITest, NoiseStrengthZero_TilesHeightEqualsBaseHeight) {
    MapGenMapData map = {};

    TerrainNoiseSettings noNoise = { 0.1f, 1.0f, 2.0f, 0.5f, 2.0f, 0.0f };
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, nullptr, nullptr, &noNoise, &map), 1);

    TerrainBaseHeights defaults = MapGenGetTerrainBaseHeights();
    for (int i = 0; i < map.tileCount; ++i) {
        const float expected = map.tiles[i].isLand ? defaults.landBaseHeight : defaults.waterBaseHeight;
        EXPECT_NEAR(map.tiles[i].height, expected, 1e-5f);
    }

    MapGenFreeMap(&map);
}

TEST(MapGenerationAPITest, CustomBaseHeights_ShiftsTileHeightsVsDefaults) {
    MapGenMapData defaultMap = {};
    MapGenMapData customMap  = {};

    TerrainNoiseSettings noNoise   = { 0.1f, 1.0f, 2.0f, 0.5f, 2.0f, 0.0f };
    TerrainBaseHeights   customBH  = { 0.9f, -0.8f };

    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, nullptr, nullptr,   &noNoise, &defaultMap), 1);
    ASSERT_EQ(MapGenGenerateMap(8, 8, 1234, 4, 0.5f, 3, nullptr, &customBH, &noNoise, &customMap),  1);

    TerrainBaseHeights defaults = MapGenGetTerrainBaseHeights();
    for (int i = 0; i < defaultMap.tileCount; ++i) {
        const bool isLand = defaultMap.tiles[i].isLand;
        const float expectedDefault = isLand ? defaults.landBaseHeight  : defaults.waterBaseHeight;
        const float expectedCustom  = isLand ? customBH.landBaseHeight  : customBH.waterBaseHeight;
        EXPECT_NEAR(defaultMap.tiles[i].height, expectedDefault, 1e-5f);
        EXPECT_NEAR(customMap.tiles[i].height,  expectedCustom,  1e-5f);
    }

    MapGenFreeMap(&defaultMap);
    MapGenFreeMap(&customMap);
}

TEST(MapGenerationAPITest, CustomParameters_AreDeterministic) {
    MapGenMapData first  = {};
    MapGenMapData second = {};

    TerrainThresholds    thresholds = { 0.1f, 0.3f, 0.5f, 0.7f };
    TerrainBaseHeights   baseHeights = { 0.6f, -0.3f };
    TerrainNoiseSettings noiseSettings = { 0.2f, 0.8f, 1.5f, 0.4f, 2.5f, 0.3f };

    ASSERT_EQ(MapGenGenerateMap(6, 6, 42, 4, 0.5f, 2, &thresholds, &baseHeights, &noiseSettings, &first),  1);
    ASSERT_EQ(MapGenGenerateMap(6, 6, 42, 4, 0.5f, 2, &thresholds, &baseHeights, &noiseSettings, &second), 1);
    ASSERT_EQ(first.tileCount, second.tileCount);

    for (int i = 0; i < first.tileCount; ++i) {
        EXPECT_EQ(first.tiles[i].q,              second.tiles[i].q);
        EXPECT_EQ(first.tiles[i].r,              second.tiles[i].r);
        EXPECT_EQ(first.tiles[i].isLand,         second.tiles[i].isLand);
        EXPECT_FLOAT_EQ(first.tiles[i].height,   second.tiles[i].height);
        EXPECT_EQ(first.tiles[i].terrain,        second.tiles[i].terrain);
    }

    MapGenFreeMap(&first);
    MapGenFreeMap(&second);
}
