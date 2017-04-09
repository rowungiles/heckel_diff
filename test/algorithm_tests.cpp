/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 */

#include "gtest/gtest.h"
#include "../src/hd_algorithm.hpp"

#include <vector>
#include <unordered_map>
#include <string>

static const std::string INSERTED = "inserted";
static const std::string DELETED = "deleted";
static const std::string MOVED = "moved";
static const std::string UNCHANGED = "unchanged";

static auto components_seperated_by_delimiter(const char delimiter, const std::string string) {

    uint32_t i = 0;

    std::string tmp;
    std::vector<std::string> s;

    for (const auto &character : string) {

        if (character != delimiter) {
            tmp.insert(i, 1, character);
            i+=1;
        } else {
            s.push_back(tmp);
            tmp = "";
            i=0;
        }
    }

    s.push_back(tmp);

    return s;
}

template <typename T>
void checkExpectedType(const std::vector<T> *result, const std::vector<T> actual) {

    if (result == nullptr) {
        return;
    }

    auto expected = *result;

    EXPECT_EQ(expected.size(), actual.size());

    for (uint32_t i = 0; i < actual.size(); ++i) {
        EXPECT_EQ(expected[i], actual[i]);
    }
}

//  TODO: really, what I want is optionals. But can't use Boost on macOS Sierra right now, so pointers it is
template <typename T>
void testExpectations(const std::vector<T> &original, const std::vector<T> &updated,
                      std::vector<T> *expected_inserted,
                      std::vector<T> *expected_deleted,
                      std::vector<T> *expected_moved,
                      std::vector<T> *expected_unchanged) {

    HeckelDiff::Algorithm<T> h;

    auto actual = h.diff(original, updated);

    auto actual_inserted = actual[INSERTED];
    auto actual_deleted = actual[DELETED];
    auto actual_moved = actual[MOVED];
    auto actual_unchanged = actual[UNCHANGED];

    checkExpectedType<T>(expected_inserted, actual_inserted);
    checkExpectedType<T>(expected_deleted, actual_deleted);
    checkExpectedType<T>(expected_moved, actual_moved);
    checkExpectedType<T>(expected_unchanged, actual_unchanged);
}

TEST(HeckelDiff, CharactersInserted) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "B", "C", "D", "E"};

    auto expected_inserted = new std::vector<std::string> {"B"};

    testExpectations<std::string>(original, updated, expected_inserted, nullptr, nullptr, nullptr);

    delete expected_inserted;
}

TEST(HeckelDiff, CharactersDeleted) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "B", "C", "D", "E"};

    auto expected_deleted = new std::vector<std::string> {"A", "X", "Y", "W", "E"};

    testExpectations<std::string>(original, updated, nullptr, expected_deleted, nullptr, nullptr);

    delete expected_deleted;
}

TEST(HeckelDiff, CharactersMoved) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "B", "C", "D", "E"};

    auto expected_moved = new std::vector<std::string> {"A", "D", "E"};

    testExpectations<std::string>(original, updated, nullptr, nullptr, expected_moved, nullptr);

    delete expected_moved;
}

TEST(HeckelDiff, CharactersUnchanged) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "B", "C", "D", "E"};

    auto expected_unchanged = new std::vector<std::string> {"C"};

    testExpectations<std::string>(original, updated, nullptr, nullptr, nullptr, expected_unchanged);

    delete expected_unchanged;
}

TEST(HeckelDiff, NoChange) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};

    auto expected_unchanged = new std::vector<std::string> {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};

    testExpectations<std::string>(original, updated, nullptr, nullptr, nullptr, expected_unchanged);

    delete expected_unchanged;
}

TEST(HeckelDiff, CompleteChangeUpdate) {

    std::vector<std::string> original {};
    std::vector<std::string> updated {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};

    auto expected_inserted = new std::vector<std::string> {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};

    testExpectations<std::string>(original, updated, expected_inserted, nullptr, nullptr, nullptr);

    delete expected_inserted;
}

TEST(HeckelDiff, CompleteChangeOriginal) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {};

    auto expected_deleted = new std::vector<std::string> {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};

    testExpectations<std::string>(original, updated, nullptr, expected_deleted, nullptr, nullptr);

    delete expected_deleted;
}

TEST(HeckelDiff, BlockMove) {

    std::vector<std::string> original{"A", "B", "C", "D", "E", "F", "G", "H"};
    std::vector<std::string> updated{"E", "F", "G", "H", "A", "B", "C", "D"};

    auto expected_moved = new std::vector<std::string>{"E", "F", "G", "H", "A", "B", "C", "D"};

    testExpectations<std::string>(original, updated, nullptr, nullptr, expected_moved, nullptr);

    delete expected_moved;
}

//  MARK: Mixture of Scenarios
TEST(HeckelDiff, MixtureOfAllScenariosAndVariableLength) {

    std::vector<std::string> original {"A", "X", "C", "Y", "D", "W", "E", "A", "E"};
    std::vector<std::string> updated {"A", "B", "C", "D", "E", "A", "Y", "Y"};

    auto expected_deleted = new std::vector<std::string> {"X", "W", "E"};
    auto expected_inserted = new std::vector<std::string> {"B", "Y"};
    auto expected_moved = new std::vector<std::string> {"D", "E", "A", "Y"};
    auto expected_unchanged = new std::vector<std::string> {"A", "C"};

    testExpectations<std::string>(original, updated, expected_inserted, expected_deleted, expected_moved, expected_unchanged);

    delete expected_deleted;
}

// MARK: Testing the reference manual - http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
TEST(HeckelDiff, ReferenceManualInserted) {

    std::string o = "much writing is like snow , a mass of long words and phrases falls upon the relevant facts covering up the details .";
    std::string n= "a mass of latin words falls upon the relevant facts like soft snow , covering up the details .";

    std::vector<std::string> original = components_seperated_by_delimiter(' ', o);
    std::vector<std::string> updated = components_seperated_by_delimiter(' ', n);

    auto expected_inserted = new std::vector<std::string> {"latin", "soft"};

    testExpectations<std::string>(original, updated, expected_inserted, nullptr, nullptr, nullptr);

    delete expected_inserted;
}

TEST(HeckelDiff, ReferenceManualDeleted) {

    std::string o = "much writing is like snow , a mass of long words and phrases falls upon the relevant facts covering up the details .";
    std::string n= "a mass of latin words falls upon the relevant facts like soft snow , covering up the details .";

    std::vector<std::string> original = components_seperated_by_delimiter(' ', o);
    std::vector<std::string> updated = components_seperated_by_delimiter(' ', n);

    auto expected_deleted = new std::vector<std::string> {"much", "writing", "is", "long", "and", "phrases"};

    testExpectations<std::string>(original, updated, nullptr, expected_deleted, nullptr, nullptr);

    delete expected_deleted;
}

TEST(HeckelDiff, ReferenceManualMoved) {

    std::string o = "much writing is like snow , a mass of long words and phrases falls upon the relevant facts covering up the details .";
    std::string n = "a mass of latin words falls upon the relevant facts like soft snow , covering up the details .";

    std::vector<std::string> original = components_seperated_by_delimiter(' ', o);
    std::vector<std::string> updated = components_seperated_by_delimiter(' ', n);

    auto expected_moved = new std::vector<std::string> {"a", "mass", "of", "words", "falls", "upon", "the", "relevant", "facts", "like", "snow", ",", "covering", "up", "the", "details", "."};

    testExpectations<std::string>(original, updated, nullptr, nullptr, expected_moved, nullptr);

    delete expected_moved;
}

TEST(HeckelDiff, ReferenceManualUnchanged) {

    std::string o = "much writing is like snow , a mass of long words and phrases falls upon the relevant facts covering up the details .";
    std::string n= "a mass of latin words falls upon the relevant facts like soft snow , covering up the details .";

    std::vector<std::string> original = components_seperated_by_delimiter(' ', o);
    std::vector<std::string> updated = components_seperated_by_delimiter(' ', n);

    auto expected_unchanged = new std::vector<std::string> {};

    testExpectations<std::string>(original, updated, nullptr, nullptr, nullptr, expected_unchanged);

    delete expected_unchanged;
}

TEST(HeckelDiff, IGListKitWhenDiffingEmptyArraysThatResultHasNoChanges) {

    std::vector<uint32_t> original {};
    std::vector<uint32_t> updated {};

    auto expected = new std::vector<uint32_t> {};

    testExpectations<uint32_t>(original, updated, expected, expected, expected, expected);

    delete expected;
}

TEST(HeckelDiff, IGListKitWhenDiffingFromEmptyArrayThatResultHasChanges) {

    std::vector<uint32_t> original {};
    std::vector<uint32_t> updated {1};

    auto expected = new std::vector<uint32_t> {1};

    testExpectations<uint32_t>(original, updated, expected, nullptr, nullptr, nullptr);

    delete expected;
}

TEST(HeckelDiff, IGListKitWhenSwappingObjectsThatResultHasMoves) {

    std::vector<uint32_t> original {1, 2};
    std::vector<uint32_t> updated {2, 1};

    auto expected = new std::vector<uint32_t> {2, 1};

    testExpectations<uint32_t>(original, updated, nullptr, nullptr, expected, nullptr);

    delete expected;
}

TEST(HeckelDiff, IGListKitWhenMovingObjectsTogetherThatResultHasMoves) {

    std::vector<uint32_t> original {1, 2, 3, 3, 4};
    std::vector<uint32_t> updated {2, 3, 1, 3, 4};

    auto expected = new std::vector<uint32_t> {2, 3, 1};

    testExpectations<uint32_t>(original, updated, nullptr, nullptr, expected, nullptr);

    delete expected;
}

TEST(HeckelDiff, IGListKitWhenDeletingItemsWithInsertsWithMovesThatResultHasInsertsMovesAndDeletes) {

    std::vector<uint32_t> original {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<uint32_t> updated  {0, 2, 3, 4, 7, 6, 9, 5, 10};

    auto expected_inserted = new std::vector<uint32_t> {9, 10};
    auto expected_deleted = new std::vector<uint32_t> {1, 8};
    auto expected_moved = new std::vector<uint32_t> {2, 3, 4, 7, 6, 5};

    testExpectations<uint32_t>(original, updated, expected_inserted, expected_deleted, expected_moved, nullptr);

    delete expected_inserted;
    delete expected_deleted;
    delete expected_moved;
}

TEST(HeckelDiff, IGListKitWhenInsertingObjectsWithArrayOfEqualObjectsThatChangeCountMatches) {

    std::vector<std::string> original {"dog", "dog"};
    std::vector<std::string> updated  {"dog", "dog", "dog", "dog"};

    auto expected = new std::vector<std::string> {"dog", "dog"};

    testExpectations<std::string>(original, updated, expected, nullptr, nullptr, nullptr);

    delete expected;
}

TEST(HeckelDiff, IGListKitWhenDeletingObjectsWithArrayOfEqualObjectsThatChangeCountMatches) {

    std::vector<std::string> original {"dog", "dog", "dog", "dog"};
    std::vector<std::string> updated  {"dog", "dog"};

    auto expected = new std::vector<std::string> {"dog", "dog"};

    testExpectations<std::string>(original, updated, nullptr, expected, nullptr, nullptr);

    delete expected;
}
