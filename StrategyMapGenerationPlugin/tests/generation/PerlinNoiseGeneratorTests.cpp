#include <gtest/gtest.h>
#include <set>
#include <algorithm>
#include "generation/PerlinNoiseGenerator.h"

TEST(PerlinNoiseGeneratorTest, PermutationSizeIsCorrect) {
    PerlinNoiseGenerator generator(1234);

    ASSERT_EQ(generator.GetPermutation().size(), 512);
}

TEST(PerlinNoiseGeneratorTest, PermutationTableSecondHalfIsDuplicate) {
    PerlinNoiseGenerator generator(1234);
    std::vector<int> permutations = generator.GetPermutation();

    for (size_t i = 0; i < 256; ++i) {
        ASSERT_EQ(permutations[i], permutations[i + 256]);
    }
}

TEST(PerlinNoiseGeneratorTest, DeterministicPermutation) {
    PerlinNoiseGenerator gen1(1234);
    PerlinNoiseGenerator gen2(1234);

    ASSERT_EQ(gen1.GetPermutation(), gen2.GetPermutation());

}

TEST(PerlinNoiseGeneratorTest, DifferentSeedsProduceDifferentResults) {
    PerlinNoiseGenerator gen1(1234);
    PerlinNoiseGenerator gen2(5678);

    ASSERT_NE(gen1.GetPermutation(), gen2.GetPermutation());
}

TEST(PerlinNoiseGeneratorTest, PermutationContainsAllValues) {
    PerlinNoiseGenerator generator(1234);
    std::vector<int> permutations = generator.GetPermutation();
    std::set<int> uniqueValues(permutations.begin(), permutations.end());
    ASSERT_EQ(uniqueValues.size(), 256);

    for (int i = 0; i < 256; ++i) {
        ASSERT_GE(permutations[i], 0);
        ASSERT_LE(permutations[i], 255);
    }
}

TEST(NoiseGeneratorTest, LinearInterpolation) {
    PerlinNoiseGenerator noise(1234);

    EXPECT_FLOAT_EQ(noise.Lerp(0.0f, 10.0f, 20.0f), 10.0f);
    EXPECT_FLOAT_EQ(noise.Lerp(1.0f, 10.0f, 20.0f), 20.0f);

    EXPECT_FLOAT_EQ(noise.Lerp(0.5f, 0.0f, 100.0f), 50.0f);
    EXPECT_FLOAT_EQ(noise.Lerp(0.5f, 10.0f, 20.0f), 15.0f);

    EXPECT_FLOAT_EQ(noise.Lerp(0.5f, 20.0f, 10.0f), 15.0f);

    EXPECT_FLOAT_EQ(noise.Lerp(2.0f, 10.0f, 20.0f), 30.0f);
}

TEST(PerlinNoiseGeneratorTest, FadeFunction) {
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Fade(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Fade(1.0f), 1.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Fade(0.5f), 0.5f);
    ASSERT_LT(PerlinNoiseGenerator::Fade(0.1f), 0.1f);
    ASSERT_GT(PerlinNoiseGenerator::Fade(0.9f), 0.9f);
}

TEST(PerlinNoiseGeneratorTest, GradFunction) {
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(0, 1.0f, 1.0f), 2.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(0, -1.0f, -1.0f), -2.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(1, -1.0f, 1.0f), 2.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(2, 1.0f, -1.0f), 2.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(3, -1.0f, -1.0f), 2.0f);
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(0, -1.0f, 1.0f), 0.0f);
}

TEST(PerlinNoiseGeneratorTest, HashBitmasking) {
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(252, 1.0f,1.0f), PerlinNoiseGenerator::Grad(0, 1.0f, 1.0f));
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(255, 1.0f,1.0f), PerlinNoiseGenerator::Grad(3, 1.0f, 1.0f));
    ASSERT_FLOAT_EQ(PerlinNoiseGenerator::Grad(256, 1.0f,1.0f), PerlinNoiseGenerator::Grad(0, 1.5f, 0.5f));
}

TEST(NoiseGeneratorTest, OutputRangeDeterministic) {
    PerlinNoiseGenerator gen1(42);
    PerlinNoiseGenerator gen2(42);
    PerlinNoiseGenerator gen3(999);

    float val1 = gen1.Noise(10.5f, 20.2f);
    float val2 = gen2.Noise(10.5f, 20.2f);
    float val3 = gen3.Noise(10.5f, 20.2f);

    EXPECT_FLOAT_EQ(val1, val2);
    EXPECT_NE(val1, val3);

    for (float i = 0; i < 100.0f; i += 0.1f) {
        float noise = gen1.Noise(i, i * 2.0f);
        EXPECT_GE(noise, -1.0f);
        EXPECT_LE(noise, 1.0f);
    }
}

TEST(NoiseGeneratorTest, Smoothness) {
    PerlinNoiseGenerator gen(123);
    float valA = gen.Noise(5.0f, 5.0f);
    float valB = gen.Noise(5.001f, 5.0f);

    float diff = std::abs(valA - valB);
    EXPECT_LT(diff, 0.01f) << "El soroll no és suau (continu)!";
}