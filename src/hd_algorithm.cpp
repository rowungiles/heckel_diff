/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 */

#include "hd_algorithm.hpp"

namespace HeckelDiff {
/*
 * TODO(rowun): Space and time efficency checks. Benchmarking. General code review.
 * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md
*/

/*
 * http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
 * This algorithm handles dupes poorly. Be sure to operate on values that can provide a hash.
 */

//  Pass 1: Put new text into entry table
    template<typename T>
    void Algorithm<T>::pass1(const std::vector<T> &n,
                              std::unordered_map<T, Entry *> &symbol_table,
                              std::vector<Record<T>> &na) {

        for (const auto &item : n) {

            auto entry = symbol_table[item] ?: new Entry();

            entry->nc += 1;
            entry->all_old_indexes.push(NotFound);

            symbol_table[item] = entry;

            na.push_back(Record<T>(entry, item));
        }
    }

//  Pass 2: Put old text into entry table
    template<typename T>
    void Algorithm<T>::pass2(const std::vector<T> &o,
                              std::unordered_map<T, Entry *> &symbol_table,
                              std::vector<Record<T>> &oa) {

        uint32_t i = 0;

        for (auto it = o.end(); o.begin() != it; --it) {

            auto item = o[i];

            auto entry = symbol_table[item] ?: new Entry();

            entry->oc += 1;
            entry->all_old_indexes.push(i);

            symbol_table[item] = entry;

            oa.push_back(Record<T>(entry, item));

            i += 1;
        }
    }

//  Pass 3: Find unaltered items
/*
 * Observation 1
 * If a line occurs only once in each file, then it must be the same line, although it may have been moved.
 * We use this observation to locate unaltered lines that we subsequently exclude from further treatment.
 */

    template<typename T>
    void Algorithm<T>::pass3(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        uint32_t new_index = 0;

        for (auto &record : na) {

            if (new_index > oa.size()) {
                return;
            }

            auto entry = record.entry;

            auto old_index = entry->all_old_indexes.top();

            entry->all_old_indexes.pop();

            if (new_index != NotFound && old_index != NotFound && na[new_index] == oa[old_index]) {

                if (new_index >= na.size() || new_index >= oa.size()) {
                    return;
                }

                na[new_index].set_index(old_index);
            }

            if (entry->nc == entry->oc && entry->oc > 0) {

                oa[old_index].set_index(new_index);
                na[new_index].set_index(old_index);
            }

            new_index += 1;
        }
    }

    enum Direction {
        Ascending,
        Descending
    };

    template<typename T>
    static void find_unchanged_blocks(const Record<T> &record, const Direction direction, const uint32_t i,
                                      std::vector<Record<T>> &na,
                                      std::vector<Record<T>> &oa) {

        if (na.size() == 0 || oa.size() == 0) {
            return;
        }

        switch (record.type) {

            case Record<T>::LineNumber: {

                auto index = record.index();

                auto offset = direction == Ascending ? 1 : -1;

                auto new_index = i + offset;
                auto old_index = index + offset;

                if (new_index >= na.size() || new_index >= oa.size()) {
                    return;
                }

                if (old_index >= na.size() || old_index >= oa.size()) {
                    return;
                }

                if (na[new_index] != oa[old_index]) {
                    return;
                }

                na[new_index].set_index(old_index);
                oa[old_index].set_index(new_index);

                break;
            }

            case Record<T>::SymbolTableEntry:
            default:
                break;
        }
    }

/*
 * pass4 & pass5
 *
 * Observation 2
 * If a line has been found to be unaltered, and the lines immediately adjacent to it in both files are identical,
 * then these lines must be the same line. This information can be used to find blocks of unchanged lines.
 */

//  Pass 4: Find ascending connected blocks
    template<typename T>
    void Algorithm<T>::pass4(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        uint32_t i = 0;

        for (auto &record : na) {

            find_unchanged_blocks(record, Ascending, i, na, oa);

            i += 1;
        }
    }

//  Pass 5: Find descending connected blocks
    template<typename T>
    void Algorithm<T>::pass5(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        for (auto it = na.end(); it != na.begin(); --it) {

            uint32_t i = std::distance(na.begin(), it - 1);

            find_unchanged_blocks(na[i], Descending, i, na, oa);
        }
    }

    template<typename T>
    static void populate_deleted_items(const std::vector<Record<T>> &oa, std::vector<T> &deleted) {

        std::vector<uint32_t> counter(oa.size(), 0);

        for (const auto &record : oa) {

            auto entry = record.entry;

            auto index = entry->all_old_indexes.top();

            if (record.index() != NotFound || index == NotFound) {
                continue;
            }

            counter[entry->all_old_indexes.top()] += 1;

            auto record_count = counter[entry->all_old_indexes.top()];

            if (record_count > entry->nc || entry->nc == 0) {
                deleted.push_back(record.value);
            }
        }
    }

    template<typename T>
    static void populate_new_items(std::vector<Record<T>> &na, std::vector<Record<T>> &oa,
                                   std::vector<T> &inserted,
                                   std::vector<T> &unchanged,
                                   std::vector<T> &moved) {

        uint32_t i = 0;

        for (auto &record : na) {

            if (record.index() == NotFound) {

                inserted.push_back(record.value);

            } else {

                if (record == oa[i]) {

                    unchanged.push_back(record.value);

                } else if (record.entry == oa[record.index()].entry) {

                    moved.push_back(record.value);
                }
            }

            i += 1;
        }
    }

    template<typename T>
    std::unordered_map<std::string, std::vector<T>> Algorithm<T>::pass6(std::vector<Record<T>> &na,
                                                                         std::vector<Record<T>> &oa) {

        std::vector<T> inserted;
        std::vector<T> unchanged;
        std::vector<T> moved;
        std::vector<T> deleted;

        populate_new_items(na, oa, inserted, unchanged, moved);
        populate_deleted_items(oa, deleted);

        std::unordered_map<std::string, std::vector<T>> results{
                {INSERTED,  inserted},
                {DELETED,   deleted},
                {MOVED,     moved},
                {UNCHANGED, unchanged}
        };

        return results;
    }

    template class Algorithm<std::string>;
    template struct Record<std::string>;

    template class Algorithm<uint32_t>;
    template struct Record<uint32_t>;
}