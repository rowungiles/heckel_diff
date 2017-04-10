/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 */

#ifndef helpers
#define helpers

#include <string>
#include <vector>

namespace HeckelDiffHelpers {

    static auto components_seperated_by_delimiter(const std::string string, const char delimiter) {

        uint32_t i = 0;

        std::string tmp;
        std::vector<std::string> s;

        for (const auto &character : string) {

            if (character != delimiter) {
                tmp.insert(i, 1, character);
                i += 1;
            } else {
                s.push_back(tmp);
                tmp = "";
                i = 0;
            }
        }

        s.push_back(tmp);

        return s;
    }

#endif //helpers
}
