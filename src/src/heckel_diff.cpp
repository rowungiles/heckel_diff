/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 * http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
 */

#include "heckel_diff.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <limits>
#include <memory>
#include <functional>
#include <tuple>

namespace HeckelDiff {

    // Pass 1 & 2: Index the items being diffed
    template<typename T>
    void Algorithm<T>::index_item(const T &item,
                                  std::unordered_map<T, std::unique_ptr<Entry>> &symbol_table,
                                  std::function<void(Entry *&entry, const T &l_item)> &lambda) {

        std::unique_ptr<Entry> entry_ptr = nullptr;
        Entry *entry;
        auto is_new_item = false;

        if (symbol_table[item]) {

            entry = symbol_table[item].get();

        } else {

            entry_ptr = std::make_unique<Entry>();
            entry = entry_ptr.get();
            is_new_item = true;
        }

        lambda(entry, item);

        if (is_new_item) {
            symbol_table[item] = std::move(entry_ptr);
        }
    }

    // Pass 1: Put new text into entry table
    template<typename T>
    void Algorithm<T>::pass1(const std::vector<T> &n,
                             std::unordered_map<T, std::unique_ptr<Entry>> &symbol_table,
                             std::vector<Record<T>> &na) {

        std::function<void(Entry *&entry, const T &item)> l = [&na](Entry *&entry, const T &l_item) {

            entry->nc += 1;
            entry->all_old_indexes.push(NotFound);

            na.push_back(Record<T>(entry, l_item));
        };

        for (const auto &item : n) {
            index_item(item, symbol_table, l);
        }
    }

    // Pass 2: Put old text into entry table
    template<typename T>
    void Algorithm<T>::pass2(const std::vector<T> &o,
                             std::unordered_map<T, std::unique_ptr<Entry>> &symbol_table,
                             std::vector<Record<T>> &oa) {

        size_t i = 0;

        std::function<void(Entry *&entry, const T &item)> l = [&oa, &i](Entry *&entry, const T &l_item) {

            entry->oc += 1;
            entry->all_old_indexes.push(i);

            oa.push_back(Record<T>(entry, l_item));

            i += 1;
        };

        // go backwards to preserve the order of stack indexes
        for (auto it = o.end(); o.begin() != it; --it) {
            index_item(o[i], symbol_table, l);
        }
    }

    // Pass 3: Find unaltered items
    /*
     * Observation 1
     * If a line occurs only once in each file, then it must be the same line, although it may have been moved.
     * We use this observation to locate unaltered lines that we subsequently exclude from further treatment.
     */
    template<typename T>
    void Algorithm<T>::pass3(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        size_t new_index = 0;

        for (const auto &record : na) {

            if (new_index >= oa.size()) {
                return;
            }

            auto &entry = record.entry;

            auto old_index = entry->all_old_indexes.top();
            entry->all_old_indexes.pop();

            // if we find an item that has moved but that may have had variance from oc to nc, allow a reverse lookup
            if (old_index != NotFound && na[new_index] == oa[old_index]) {

                na[new_index].set_index(old_index);
            }

            if (entry->nc == entry->oc && entry->oc > 0) {

                oa[old_index].set_index(new_index);
                na[new_index].set_index(old_index);
            }

            new_index += 1;
        }
    }

    template<typename T>
    void Algorithm<T>::find_unchanged_blocks(const Record<T> &record, const Direction &direction, const size_t &i,
                                      std::vector<Record<T>> &na,
                                      std::vector<Record<T>> &oa) {

        switch (record.type) {

            case Record<T>::LineNumber: {

                const auto &index = record.index();

                const auto new_index = i + direction;
                const auto old_index = index + direction;

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

    // Pass 4 & 5: Find blocks of unchanged lines.
    /*
     * Observation 2
     * If a line has been found to be unaltered, and the lines immediately adjacent to it in both files are identical,
     * then these lines must be the same line. This information can be used to find blocks of unchanged lines.
     */

    // Pass 4: Find ascending connected blocks
    template<typename T>
    void Algorithm<T>::pass4(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        size_t i = 0;

        if (na.empty() || oa.empty()) {
            return;
        }

        for (const auto &record : na) {

            find_unchanged_blocks(record, Ascending, i, na, oa);

            i += 1;
        }
    }

    //  Pass 5: Find descending connected blocks
    template<typename T>
    void Algorithm<T>::pass5(std::vector<Record<T>> &na, std::vector<Record<T>> &oa) {

        if (na.empty() || oa.empty()) {
            return;
        }

        for (auto j = na.size()-1; j != 0; --j) {

            find_unchanged_blocks(na[j], Descending, j, na, oa);
        }
    }

    template<typename T>
    std::vector<T> Algorithm<T>::populate_deleted_items(const std::vector<Record<T>> &oa) {

        std::vector<T> deleted;

        // keep track of the number of times an item is deleted. Use this count to avoid deleting duplicates.
        std::vector<size_t> counter(oa.size(), 0);

        for (const auto &record : oa) {

            const auto &entry = record.entry;

            const auto &old_index = entry->all_old_indexes.top();

            if (old_index == NotFound) {
                continue;
            }

            counter[old_index] += 1;

            const auto &record_count = counter[old_index];

            if (record_count > entry->nc || entry->nc == 0) {
                deleted.push_back(record.value);
            }
        }

        return deleted;
    }

    template<typename T>
    auto Algorithm<T>::populate_new_items(const std::vector<Record<T>> &na, const std::vector<Record<T>> &oa) {

        std::vector<T> inserted, moved, unchanged;

        size_t i = 0;

        for (const auto &record : na) {

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

        return std::make_tuple(inserted, moved, unchanged);
    }

    template<typename T>
    const std::unordered_map<std::string, std::vector<T>> Algorithm<T>::pass6(std::vector<Record<T>> &na,
                                                                        std::vector<Record<T>> &oa) {

        const auto updates = populate_new_items(na, oa);
        const auto deleted = populate_deleted_items(oa);

        const std::unordered_map<std::string, std::vector<T>> results {
                {INSERTED,  std::get<0>(updates)},
                {MOVED,     std::get<1>(updates)},
                {UNCHANGED, std::get<2>(updates)},
                {DELETED,   deleted}
        };

        return results;
    }

    template class Algorithm<std::string>;
    template struct Record<std::string>;

    template class Algorithm<size_t>;
    template struct Record<size_t>;

}  // namespace HeckelDiff
