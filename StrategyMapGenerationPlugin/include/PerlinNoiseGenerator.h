#pragma once
#include <vector>
#include "CoreAPI.h"

class PerlinNoiseGenerator {
public:
    PerlinNoiseGenerator(int seed);
    std::vector<int> GetPermutation() const;

    float Lerp(float t, float a, float b) const;

    static float Fade(float t);
    static float Grad(int hash, float x, float y);
    float Noise(float x, float y);

private:
    int seed;
    std::vector<int> permutation;
};

