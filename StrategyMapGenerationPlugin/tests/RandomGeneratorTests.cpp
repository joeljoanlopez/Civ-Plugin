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