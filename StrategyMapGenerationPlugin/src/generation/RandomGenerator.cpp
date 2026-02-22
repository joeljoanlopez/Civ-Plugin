#include "generation/RandomGenerator.h"

#include <list>

RandomGenerator::RandomGenerator(int seed) {
    this->seed = seed;
    this->rng = std::mt19937(seed);
}

std::list<int> RandomGenerator::GenerateListBetween(int min, int max, int size) {
    std::list<int> result;
    
    for (int i = 0; i < size; ++i) {
        result.push_back(GenerateIntInRange(min, max));
    }

    return result;
}

float RandomGenerator::RandomNumberInRange(float min, float max) {
    float normalized = static_cast<float>(rng()) / static_cast<float>(rng.max());
    
    return min + normalized * (max - min);
}

int RandomGenerator::GenerateIntInRange(int min, int max) {
    if (min > max) {
        const int temp = min;
        min = max;
        max = temp;
    }
    
    unsigned long range = static_cast<unsigned long>(max - min) + 1;
    return min + static_cast<int>(rng() % range);
}

std::mt19937 RandomGenerator::GetEngine() const {
    return rng;
}
