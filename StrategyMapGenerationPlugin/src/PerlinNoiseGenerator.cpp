#include "PerlinNoiseGenerator.h"
#include <cmath>
#include <random>

PerlinNoiseGenerator::PerlinNoiseGenerator(int seed) : seed(seed) {
    permutation.resize(256);
    for (int i = 0; i < 256; ++i) {
        permutation[i] = i;
    }

    std::mt19937 rng(seed);
    for (int i = 255; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng);
        std::swap(permutation[i], permutation[j]);
    }

    permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

std::vector<int> PerlinNoiseGenerator::GetPermutation() const {
    return permutation;
}

float PerlinNoiseGenerator::Lerp(float t, float a, float b) const {
    return a + t * (b - a);
}

float PerlinNoiseGenerator::Fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoiseGenerator::Grad(int hash, float dx, float dy) {
    int h = hash & 3; // Obtenim la direccio dividint el hash en 4 [0, 1, 2, 3]

    // 2. Determinació del signe de dx
    // Si l'últim bit és 0 (h és 0 o 2), dx positiu.
    // Si l'últim bit és 1 (h és 1 o 3), dx negatiu.
    float u = (h & 1) == 0 ? dx : -dx;

    // 3. Determinació del signe de dy
    // Si el penúltim bit és 0 (h és 0 o 1), dy positiu.
    // Si el penúltim bit és 1 (h és 2 o 3), dy negatiu.
    float v = (h & 2) == 0 ? dy : -dy;


    return u + v;
}

float PerlinNoiseGenerator::Noise(float x, float y) {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float fadedX = Fade(x);
    float fadedY = Fade(y);

    int left = permutation[X] + Y;
    int right = permutation[X + 1] + Y;

    int topLeft = permutation[left];
    int botLeft = permutation[left + 1];
    int topRight = permutation[right];
    int botRight = permutation[right + 1];

    float result1 = Lerp(fadedX, Grad(topLeft, x, y), Grad(topRight, x-1, y));
    float result2 = Lerp(fadedX, Grad(botLeft, x, y-1), Grad(botRight, x-1, y-1));

    return Lerp(fadedY, result1, result2);
}