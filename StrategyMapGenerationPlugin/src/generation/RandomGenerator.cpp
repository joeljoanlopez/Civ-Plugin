#include "generation/RandomGenerator.h"

#include <limits>
#include <list>
#include <vector>

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

// Precondition: min <= max. If min > max, arguments are silently swapped.
float RandomGenerator::RandomNumberInRange(float min, float max) {
    if (min > max) std::swap(min, max);
    constexpr uint32_t mantissa_range = 1u << std::numeric_limits<float>::digits;
    float normalized = static_cast<float>(rng() % mantissa_range) / static_cast<float>(mantissa_range);
    return min + normalized * (max - min);
}

// Precondition: min <= max. If min > max, arguments are silently swapped.
int RandomGenerator::GenerateIntInRange(int min, int max) {
    if (min > max) {
        std::swap(min, max);
    }
    
    uint32_t range = static_cast<uint32_t>(max - min) + 1;
    uint32_t threshold = -static_cast<int32_t>(range) % range;
    uint32_t result;
    do {
        result = rng();
    } while (result < threshold);
    return min + static_cast<int>(result % range);
}

std::mt19937 RandomGenerator::GetEngine() const {
    return rng;
}

void RandomGenerator::Shuffle(std::vector<int>& v) {
    for (int i = static_cast<int>(v.size()) - 1; i > 0; --i) {
        int j = GenerateIntInRange(0, i);
        std::swap(v[i], v[j]);
    }
}

void RandomGenerator::Shuffle(std::vector<bool>& v) {
    for (int i = static_cast<int>(v.size()) - 1; i > 0; --i) {
        int j = GenerateIntInRange(0, i);
        bool tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}
