#include "algorithm.hpp"
#include <algorithm>
#include <iostream>

/* http://documents.scribd.com/docs/10ro9oowpo1h81pgh1as.pdf
 * This algorithm handles dupes poorly. Be sure to operate on values that can provide a hash.
 */

static const std::string INSERTED = "inserted";
static const std::string DELETED = "deleted";
static const std::string MOVED = "moved";
static const std::string UNCHANGED = "unchanged";

//  Pass 1: Put new text into symbol table
template<typename T>
void HeckelDiff<T>::pass1(const std::vector<T> &n, std::unordered_map<T, SymbolTableEntry*> &symbol_table, std::vector<Entry<T>> &na) {

    for (const auto &item : n) {

        auto entry = symbol_table[item] ? : new SymbolTableEntry();

        entry->nc_increment();

        symbol_table[item] = entry;

        na.push_back(Entry<T>(entry, item));
    }
}

//  Pass 2: Put old text into symbol table
template<typename T>
void HeckelDiff<T>::pass2(const std::vector<T> &o, std::unordered_map<T, SymbolTableEntry*> &symbol_table, std::vector<Entry<T>> &oa) {

    uint32_t i = 0;

    for (const auto &item : o) {

        auto entry = symbol_table[item] ? : new SymbolTableEntry();

        entry->oc_increment();

        entry->update_olno(i);

        symbol_table[item] = entry;

        oa.push_back(Entry<T>(entry, item));

        i+=1;
    }
}

//  Pass 3: Find unaltered items
/*
 * Observation 1
 * If a line occurs only once in each file, then it must be the same line, although it may have been moved.
 * We use this observation to locate unaltered lines that we subsequently exclude from further treatment.
 */

template<typename T>
void HeckelDiff<T>::pass3(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa) {

    uint32_t i = 0;

    for (auto &entry : na) {

        if (entry.m_type == Entry<T>::SymbolEntry) {

            auto symbol = entry.m_symbol_table_entry;

            if (symbol->m_nc == symbol->m_oc == SymbolTableEntry::One) {

                oa[symbol->m_olno].set_line_number(i);
                entry.set_line_number(symbol->m_olno);
            }
        }

        i+=1;
    }
}

template<typename T>
static void find_unchanged_blocks(const Entry<T> &entry, const uint32_t direction, const uint32_t i, std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa) {

    switch (entry.m_type) {

        case Entry<T>::LineNumber: {

            if (na.size() == 0 || oa.size() == 0) {
                return;
            }

            auto line_number = entry.m_line_number;

            auto new_line_number = line_number + direction;

            if (new_line_number >= na.size() || new_line_number >= oa.size()) {
                return;
            }

            if (na[new_line_number] != oa[new_line_number]) {
                return;
            }

            na[i+direction].set_line_number(new_line_number);
            oa[new_line_number].set_line_number(i+direction);

            break;
        }

        case Entry<T>::SymbolEntry:
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
void HeckelDiff<T>::pass4(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa) {

    uint32_t i = 0;

    for (auto &entry : na) {
        find_unchanged_blocks(entry, 1, i, na, oa);
        ++i;
    }
}

//  Pass 5: Find descending connected blocks
template<typename T>
void HeckelDiff<T>::pass5(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa) {

    for (auto it = na.end(); it != na.begin(); --it) {

        uint32_t i = std::distance(na.begin(), it-1);

        if (i == 0) {
            return;
        }

        auto entry = na[i];

        find_unchanged_blocks(entry, -1, i, na, oa);
    }
}

template<typename T>
static void populate_deleted_items(const std::vector<Entry<T>> &na, const std::vector<Entry<T>> &oa, std::vector<T> &deleted) {

    uint32_t i = 0;
    auto n =na;

    for (const auto &entry : oa) {

        switch (entry.m_type) {

            case Entry<T>::SymbolEntry: {

                auto symbol = entry.m_symbol_table_entry;

                switch (symbol->m_nc) {
                    case SymbolTableEntry::Zero:
                        deleted.push_back(entry.m_value);
                        break;

                    case SymbolTableEntry::One:
                    case SymbolTableEntry::Many:

//                        if (symbol->m_olno >= na.size()) {
//
//                            deleted.push_back(entry.m_value);
//
//                        } else if (na[entry.m_line_number].m_symbol_table_entry != symbol) {
//
//                            deleted.push_back(entry.m_value);
//                        }
//                        break;

                    default:
                        break;
                }
                break;
            }

            case Entry<T>::LineNumber:
            default:
                break;
        }

        ++i;
    }
}

template<typename T>
static void populate_new_items(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa, std::vector<T> &inserted, std::vector<T> &unchanged, std::vector<T> &moved) {

    uint32_t i = 0;

    for (const auto &entry : na) {

        switch (entry.m_type) {

            case Entry<T>::SymbolEntry: {

                inserted.push_back(entry.m_value);
                break;
            }

            case Entry<T>::LineNumber: {

                auto line_number = entry.m_line_number;

//                auto symbol = entry.m_symbol_table_entry;

                if (na[line_number] == oa[line_number]) {

                    unchanged.push_back(entry.m_value);

                } else {

                    moved.push_back(entry.m_value);
                }
                break;
            }

            default:
                break;
        }

        i+=1;
    }
}

template<typename T>
std::unordered_map<std::string, std::vector<T>> HeckelDiff<T>::pass6(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa) {

    std::vector<T> inserted;
    std::vector<T> unchanged;
    std::vector<T> moved;
    std::vector<T> deleted;

    populate_new_items(na, oa, inserted, unchanged, moved);
    populate_deleted_items(na, oa, deleted);

    // TODO: Space efficency checks and get the delete to work in block chunks.
    return {
            {INSERTED, inserted},
            {DELETED, deleted},
            {MOVED, moved},
            {UNCHANGED, unchanged}
    };
}

template class HeckelDiff<std::string>;
template class Entry<std::string>;