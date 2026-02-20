#pragma once
#include <vector>
#include "CoreAPI.h"

class MAPGEN_API PerlinNoiseGenerator {
public:
    PerlinNoiseGenerator(int seed);

    std::vector<int> GetPermutation() const;

    float Lerp(float t, float a, float b) const;

    static float Fade(float t);
    static float Grad(int hash, float x, float y);
    float Noise(float x, float y) const;

private:
    int seed;
    std::vector<int> permutation;
};

