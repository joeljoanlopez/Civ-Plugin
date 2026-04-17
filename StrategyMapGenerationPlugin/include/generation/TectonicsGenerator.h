#pragma once
#include "core/CoreAPI.h"
#include "hex/HexCoord.h"
#include "hex/HexGrid.h"
#include "RandomGenerator.h"
#include <list>
#include <map>

#include "PerlinNoiseGenerator.h"

struct TerrainThresholds;
struct TerrainBaseHeights;

class MAPGEN_API TectonicsGenerator {
public:
    TectonicsGenerator(int seed);

    void GenerateTectonicPlates(HexGrid& grid, int plateCount, float landRatio = 0.5f);
    void ProcessTerrainMap(
        HexGrid& grid,
        int noiseOctaves = 3,
        const TerrainThresholds* thresholds = nullptr,
        const TerrainBaseHeights* baseHeights = nullptr
    ) const;

private:
    struct PlateCenter {
        HexCoord coord;
        int index;
        bool isLand;
    };

    RandomGenerator rng;
    PerlinNoiseGenerator noiseGen;

    [[nodiscard]] std::list<PlateCenter> GenerateTectonicCenters(int count, const HexGrid& grid, float landRatio);

    static void AssignTectonicPlates(HexGrid& grid, const std::list<PlateCenter>& centers);
};
