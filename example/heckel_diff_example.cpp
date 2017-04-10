/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 * http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
 */
#include <chrono>
#include <iostream>
#include <vector>
#include <heckel_diff/heckel_diff.hpp>

template <typename T>
static std::string vector_to_string(std::vector<T> vector) {

    std::string tmp = "";

    for (const auto &item : vector) {

        tmp += item;
    }

    return tmp;
}

int main() {

    std::vector<uint32_t>o {1, 2, 3, 4, 5};
    std::vector<uint32_t>n {3, 2, 1, 4, 6};

    HeckelDiff::Algorithm<uint32_t> heckel_diff;

    auto actual = heckel_diff.diff(o, n);

    auto inserted = actual[HeckelDiff::INSERTED];
    auto deleted = actual[HeckelDiff::DELETED];
    auto moved = actual[HeckelDiff::MOVED];
    auto unchanged = actual[HeckelDiff::UNCHANGED];

    std::cout << "\n"
              << "\nInserted :"
              << vector_to_string<uint32_t>(inserted)
              << "\nDeleted: "
              << vector_to_string<uint32_t>(deleted)
              << "\nMoved: "
              << vector_to_string<uint32_t>(moved)
              << "\nUnchanged: "
              << vector_to_string<uint32_t>(unchanged)
              << "\n";
    return 0;
}
