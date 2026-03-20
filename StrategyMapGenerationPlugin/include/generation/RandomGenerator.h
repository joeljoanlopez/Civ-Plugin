#pragma once
#include <list>
#include <random>
#include "core/CoreAPI.h"

class MAPGEN_API RandomGenerator {
public:
    RandomGenerator(int seed);

    [[nodiscard]] std::list<int> GenerateListBetween(int min, int max, int size);

    [[nodiscard]] float RandomNumberInRange(float min, float max);
    [[nodiscard]] int GenerateIntInRange(int min, int max);
    [[nodiscard]] std::mt19937 GetEngine() const;

private:
    int seed;
    std::mt19937 rng;
};