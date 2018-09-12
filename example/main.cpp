/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 * http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
 */
#include <chrono>
#include <iostream>
#include <vector>
#include <string>

#include "heckel_diff.hpp"

std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t> &vector)
{
    std::string tmp = "";

    if (!vector.empty()) {

        for (const auto &item : vector) {

            tmp += std::to_string(item) + " ";
        }
    }
    // write obj to stream
    return os << tmp;
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
              << "\nInserted: "
              << inserted
              << "\nDeleted: "
              << deleted
              << "\nMoved: "
              << moved
              << "\nUnchanged: "
              << unchanged
              << "\n";
    return 0;
}
