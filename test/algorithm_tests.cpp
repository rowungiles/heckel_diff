#include "gtest/gtest.h"
#include "../src/algorithm.hpp"

TEST(HeckelDiff, CharactersRemoved) {

    auto original = "AXCYDWEABE";
    auto updated = "ABCDE";

    auto expected = "XYWABE";

    EXPECT_TRUE(false);
}

TEST(HeckelDiff, CharactersInserted) {

    auto original = "AXCYDWEABE";
    auto updated = "ABCDE";

    auto expected = "B";

    EXPECT_TRUE(false);
}

TEST(HeckelDiff, CharactersUnchanged) {

    auto original = "AXCYDWEABE";
    auto updated = "ABCDE";

    auto expected = "ACDE";

    EXPECT_TRUE(false);
}