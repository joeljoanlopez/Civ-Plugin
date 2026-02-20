#include <gtest/gtest.h>
#include <list>
#include "RandomGenerator.h"

TEST(RandomGeneratorTest, DeterministicRandomGeneration) {
    RandomGenerator randomGenerator(1234);
    std::list<int> a = randomGenerator.GenerateListBetween(0, 1, 5);

    RandomGenerator randomGenerator2(1234);
    std::list<int> b = randomGenerator2.GenerateListBetween(0, 1, 5);

    ASSERT_EQ(a, b);
}

TEST(RandomGeneratorTest, DeterministicInt) {
    RandomGenerator randomGenerator(42);

    std::vector<int> expectedValues = {
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100),
        randomGenerator.GenerateIntInRange(0, 100)
    };

    RandomGenerator randomGenerator2(42);
    std::vector<int> actualValues = {
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100),
        randomGenerator2.GenerateIntInRange(0, 100)
    };

    ASSERT_EQ(expectedValues, actualValues);
}

TEST(RandomGeneratorTest, DeterministicFloat) {
    RandomGenerator randomGenerator(42);

    std::vector<float> expectedValues = {
        randomGenerator.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator.RandomNumberInRange(0.0f, 1.0f)
    };

    RandomGenerator randomGenerator2(42);
    std::vector<float> actualValues = {
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f),
        randomGenerator2.RandomNumberInRange(0.0f, 1.0f)
    };

    ASSERT_EQ(expectedValues, actualValues);
}

TEST(RandomGeneratorTest, DivergentRandomGeneration) {
    RandomGenerator randomGenerator(1234);
    std::list<int> a = randomGenerator.GenerateListBetween(0, 1, 5);

    RandomGenerator randomGenerator2(5678);
    std::list<int> b = randomGenerator2.GenerateListBetween(0, 1, 5);

    ASSERT_NE(a, b);
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