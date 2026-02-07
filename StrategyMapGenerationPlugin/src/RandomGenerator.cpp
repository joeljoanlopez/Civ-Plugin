#include "RandomGenerator.h"

#include <list>

RandomGenerator::RandomGenerator(int seed) {
    this->seed = seed;
    this->rng = std::mt19937(seed);
}

std::list<int> RandomGenerator::GenerateListBetween(int min, int max, int size) {
    std::list<int> result;
    std::uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < size; ++i) {
        result.push_back(dist(rng));
    }

    return result;
}
