#pragma once
#include "CoreAPI.h"
#include "HexCoord.h"
#include "HexGrid.h"
#include "RandomGenerator.h"
#include <list>


class MAPGEN_API TectonicsGenerator {
public:
    TectonicsGenerator(int seed);

    std::list<HexCoord> GenerateTectonicCenters(int count, HexGrid grid, float landRatio = 0.5f);

private:
    RandomGenerator rng;
};
