/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 */

#include "hd_algorithm.hpp"

#include "helpers.hpp"

int main() {

    std::string o = "much writing is like snow , a mass of long words and phrases falls upon the relevant facts covering up the details .";
    std::string n = "a mass of latin words falls upon the relevant facts like soft snow , covering up the details .";

    auto original = HeckelDiffHelpers::components_seperated_by_delimiter(o, ' ');
    auto updated = HeckelDiffHelpers::components_seperated_by_delimiter(n, ' ');

    HeckelDiff::Algorithm<std::string> h;

    auto actual = h.diff(original, updated);

    return 0;
}
