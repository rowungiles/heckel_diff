#include "gtest/gtest.h"
#include "../src/algorithm.hpp"
#include <string>

TEST(HeckelDiff, CharactersInserted) {

    auto original = "AXCYDWEAE";
    auto updated = "ABCDE";

    std::vector<char> expected{'A', 'B', 'E'};

    HeckelDiff<std::string, char> h;

    auto actual = h.diff(original, updated)["inserted"];

    EXPECT_EQ(expected.size(), actual.size());

    for (uint32_t i=0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]);
    }
}

TEST(HeckelDiff, CharactersRemoved) {

    auto original = "AXCYDWEABE";
    auto updated = "ABCDE";

    std::vector<char> expected{'X','Y','W','E','A'};

    HeckelDiff<std::string, char> h;

    auto actual = h.diff(original, updated)["deleted"];

    EXPECT_EQ(expected.size(), actual.size());

    for (uint32_t i=0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]);
    }
}

TEST(HeckelDiff, CharactersMoved) {

    auto original = "AXCYDWEAE";
    auto updated = "ABCDE";

    std::vector<char> expected{'D'};

    HeckelDiff<std::string, char> h;

    auto actual = h.diff(original, updated)["moved"];

    EXPECT_EQ(expected.size(), actual.size());

    for (uint32_t i=0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]);
    }
}

TEST(HeckelDiff, CharactersUnchanged) {

    auto original = "AXCYDWEAE";
    auto updated = "ABCDE";

    std::vector<char> expected{'C'};

    HeckelDiff<std::string, char> h;

    auto actual = h.diff(original, updated)["unchanged"];

    EXPECT_EQ(expected.size(), actual.size());

    for (uint32_t i=0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]);
    }
}