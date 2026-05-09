#include <gtest/gtest.h>
#include <list>
#include <vector>
#include "generation/RandomGenerator.h"

TEST(RandomGeneratorTest, DeterministicRandomGeneration) {
    RandomGenerator randomGenerator(1234);
    std::list<int> a = randomGenerator.GenerateListBetween(0, 1, 5);

    RandomGenerator randomGenerator2(1234);
    std::list<int> b = randomGenerator2.GenerateListBetween(0, 1, 5);

    ASSERT_EQ(a, b);
}

TEST(RandomGeneratorTest, DeterministicInt) {
    RandomGenerator randomGenerator(42);

    std::vector<int> randomValues1 = {
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100)
    };

    RandomGenerator randomGenerator2(42);
    std::vector<int> randomValues2 = {
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100)
    };

    ASSERT_EQ(randomValues1, randomValues2);
}

TEST(RandomGeneratorTest, DeterministicFloat) {
    RandomGenerator randomGenerator(42);

    std::vector<float> randomValues1 = {
        randomGenerator.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator.RandomNumberInRange(0.0f, 1.0f)
    };

    RandomGenerator randomGenerator2(42);
    std::vector<float> randomValues2 = {
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f)
    };

    ASSERT_EQ(randomValues1, randomValues2);
}

TEST(RandomGeneratorTest, DivergentRandomGeneration) {
    RandomGenerator randomGenerator(1234);
    std::list<int> a = randomGenerator.GenerateListBetween(0, 1, 5);

    RandomGenerator randomGenerator2(5678);
    std::list<int> b = randomGenerator2.GenerateListBetween(0, 1, 5);

    ASSERT_NE(a, b);
}

TEST(RandomGeneratorTest, DivergentIntGeneration) {
    RandomGenerator randomGenerator(1234);
    std::vector<int> randomValues1 = {
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
    };

    RandomGenerator randomGenerator2(5678);
    std::vector<int> randomValues2 = {
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
    };

    ASSERT_NE(randomValues1, randomValues2);
}

TEST(RandomGeneratorTest, DivergentFloatGeneration) {
    RandomGenerator randomGenerator(1234);
    std::vector<float> randomValues1 = {
        randomGenerator.RandomNumberInRange(0, 100),
        randomGenerator.RandomNumberInRange(0, 100),
        randomGenerator.RandomNumberInRange(0, 100),
        randomGenerator.RandomNumberInRange(0, 100),
        randomGenerator.RandomNumberInRange(0, 100),
    };

    RandomGenerator randomGenerator2(5678);
    std::vector<float> randomValues2 = {
        randomGenerator2.RandomNumberInRange(0, 100),
        randomGenerator2.RandomNumberInRange(0, 100),
        randomGenerator2.RandomNumberInRange(0, 100),
        randomGenerator2.RandomNumberInRange(0, 100),
        randomGenerator2.RandomNumberInRange(0, 100),
    };

    ASSERT_NE(randomValues1, randomValues2);
}

TEST(RandomGeneratorTest, ValueRangeVerification) {
    RandomGenerator randomGenerator(1234);
    std::list<int> values = randomGenerator.GenerateListBetween(5, 10, 1000);

    for (int value : values) {
        ASSERT_GE(value, 5.0f);
        ASSERT_LE(value, 10.0f);
    }
}

TEST(RandomGeneratorTest, CorrectListSize) {
    RandomGenerator randomGenerator(1234);
    std::list<int> values = randomGenerator.GenerateListBetween(0, 1, 100);

    ASSERT_EQ(values.size(), 100);
}

TEST(RandomGeneratorTest, EmptyListGeneration) {
    RandomGenerator randomGenerator(1234);
    std::list<int> values = randomGenerator.GenerateListBetween(0, 1, 0);

    ASSERT_TRUE(values.empty());
}

TEST(RandomGeneratorTest, SingleValueRange) {
    RandomGenerator randomGenerator(1234);
    std::list<int> values = randomGenerator.GenerateListBetween(5, 5, 100);

    for (int value : values) {
        ASSERT_FLOAT_EQ(value, 5.0f);
    }
}

TEST(RandomGeneratorTest, UniformDistribution_IntInRange) {
    RandomGenerator rng(1234);
    const int buckets = 10;
    const int samples = 10000;
    int counts[buckets] = {};

    for (int i = 0; i < samples; ++i)
        counts[rng.GenerateIntInRange(0, buckets - 1)]++;

    const int expected = samples / buckets;
    for (int b = 0; b < buckets; ++b) {
        EXPECT_GE(counts[b], expected * 8 / 10);
        EXPECT_LE(counts[b], expected * 12 / 10);
    }
}

TEST(RandomGeneratorTest, Shuffle_ProducesUniformDistribution) {
    RandomGenerator rng(1234);
    const int trials = 3000;
    int firstIsZero = 0;

    for (int i = 0; i < trials; ++i) {
        std::vector<int> v = {0, 1};
        rng.Shuffle(v);
        if (v[0] == 0) firstIsZero++;
    }

    EXPECT_GE(firstIsZero, trials * 4 / 10);
    EXPECT_LE(firstIsZero, trials * 6 / 10);
}

TEST(RandomGeneratorTest, FloatRange_StaysInBounds) {
    RandomGenerator rng(1234);
    const float lo = -5.0f, hi = 10.0f;

    for (int i = 0; i < 10000; ++i) {
        float v = rng.RandomNumberInRange(lo, hi);
        EXPECT_GE(v, lo);
        EXPECT_LT(v, hi);
    }
}