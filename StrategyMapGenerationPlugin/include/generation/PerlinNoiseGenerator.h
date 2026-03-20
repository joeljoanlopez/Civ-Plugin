#pragma once
#include <vector>
#include "core/CoreAPI.h"

class MAPGEN_API PerlinNoiseGenerator {
public:
    PerlinNoiseGenerator(int seed);

    [[nodiscard]] std::vector<int> GetPermutation() const;

    [[nodiscard]] float Lerp(float t, float a, float b) const;

    [[nodiscard]] static float Fade(float t);
    [[nodiscard]] static float Grad(int hash, float x, float y);
    [[nodiscard]] float Noise(float x, float y) const;

private:
    int seed;
    std::vector<int> permutation;
};

