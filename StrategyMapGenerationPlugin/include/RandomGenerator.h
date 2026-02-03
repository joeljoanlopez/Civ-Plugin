#pragma once
#include <list>
#include <random>
#include "CoreAPI.h"

class MAPGEN_API RandomGenerator {
public:
    RandomGenerator(int seed);

    std::list<float> GenerateListBetween(float min, float max, int size);

private:
    int seed;
    std::mt19937 rng;
};