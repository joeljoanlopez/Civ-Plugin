#pragma once
#include <list>
#include <random>
#include "core/CoreAPI.h"

class MAPGEN_API RandomGenerator {
public:
    RandomGenerator(int seed);

    std::list<int> GenerateListBetween(int min, int max, int size);

    float RandomNumberInRange(float min, float max);
    int GenerateIntInRange(int min, int max);
    std::mt19937 GetEngine() const;

private:
    int seed;
    std::mt19937 rng;
};