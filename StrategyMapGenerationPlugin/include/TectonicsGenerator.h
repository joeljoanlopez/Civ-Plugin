#pragma once
#include "CoreAPI.h"
#include "HexCoord.h"
#include "HexGrid.h"
#include "RandomGenerator.h"
#include <list>
#include <map>

#include "PerlinNoiseGenerator.h"


class MAPGEN_API TectonicsGenerator {
public:
    TectonicsGenerator(int seed);

    void GenerateTectonicPlates(HexGrid& grid, int plateCount, float landRatio = 0.5f);
    void ProcessTerrainMap(HexGrid& grid, int noiseOctaves = 3);

private:
    struct PlateCenter {
        HexCoord coord;
        int index;
        bool isLand;
    };

    RandomGenerator rng;
    PerlinNoiseGenerator noiseGen;

    std::list<PlateCenter> GenerateTectonicCenters(int count, const HexGrid& grid, float landRatio);
    void AssignTectonicPlates(HexGrid& grid, const std::list<PlateCenter>& centers);
};
