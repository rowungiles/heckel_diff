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

//  make references to all new items
template<typename T, typename U>
void HeckelDiff<T, U>::pass1(const T &n, std::unordered_map<U, SymbolTableEntry*> &symbol_table, std::vector<Entry> &na) {

    for (const auto &item : n) {

        auto entry = symbol_table[item] ? : new SymbolTableEntry();

        entry->nc_increment();
        
        symbol_table[item] = entry;

        na.push_back(Entry(entry, item));
    }
}

//  make references to all old items
template<typename T, typename U>
void HeckelDiff<T, U>::pass2(const T &o, std::unordered_map<U, SymbolTableEntry*> &symbol_table, std::vector<Entry> &oa) {

    uint32_t i = 0;

    for (const auto &item : o) {

        auto entry = symbol_table[item] ? : new SymbolTableEntry();

        entry->oc_increment();

        entry->update_olno(i);

        symbol_table[item] = entry;

        oa.push_back(Entry(entry, item));

        i+=1;
    }
}

/*
 * If a line occurs only once in each file, then it must be the same line, although it may have been moved.
 * We use this observation to locate unaltered lines that we subsequently exclude from further treatment.
 *
 * eg. find items that haven't moved
 */
template<typename T, typename U>
void HeckelDiff<T, U>::pass3(std::vector<Entry> &na, std::vector<Entry> &oa) {

    uint32_t i = 0;

    for (auto &item : na) {

        if (item.m_type == Entry::SymbolEntry) {

            auto symbol = item.m_symbol_table_entry;

            if (symbol->m_nc == symbol->m_oc == SymbolTableEntry::One) {

                oa[symbol->m_olno].set_line_number(i);
                item.set_line_number(symbol->m_olno);
            }
        }

        i+=1;
    }
}

static void process_items_ascending(Entry &item, std::vector<Entry> &na, std::vector<Entry> &oa) {

    auto line_number = item.m_line_number;

    auto old_entry = oa[line_number];
    auto old_line_number = old_entry.m_line_number;

    if (line_number != old_entry.m_line_number) {
        return;
    }

    if (na[line_number+1].m_symbol_table_entry != oa[old_line_number+1].m_symbol_table_entry) {
        return;
    }

    na[line_number+1].set_line_number(old_line_number+1);
    oa[old_line_number+1].set_line_number(line_number+1);
}

template<typename T, typename U>
void HeckelDiff<T, U>::pass4(std::vector<Entry> &na, std::vector<Entry> &oa) {

    for (auto &item : na) {

        switch (item.m_type) {
            case Entry::SymbolEntry:
                process_items_ascending(item, na, oa);
                break;
            case Entry::LineNumber:
            default:
                break;
        }
    }
}

static void process_items_decending(Entry &entry, std::vector<Entry> &na, std::vector<Entry> &oa) {

    auto line_number = entry.m_line_number;

    auto old_entry = oa[line_number];
    auto old_line_number = old_entry.m_line_number;

    if (line_number == 0 || old_line_number == 0) {
        return;
    }

    if (line_number != old_entry.m_line_number) {
        return;
    }

    if (na[line_number-1].m_symbol_table_entry != oa[old_line_number-1].m_symbol_table_entry) {
        return;
    }

    na[line_number-1].set_line_number(old_line_number-1);
    oa[old_line_number-1].set_line_number(line_number-1);
}

template<typename T, typename U>
void HeckelDiff<T, U>::pass5(std::vector<Entry> &na, std::vector<Entry> &oa) {

    uint32_t na_size = na.size();

    for (uint32_t i = na_size; i > 0; --i) {

        auto entry = na[i];
        switch (entry.m_type) {
            case Entry::SymbolEntry:
                process_items_decending(entry, na, oa);
                break;
            case Entry::LineNumber:
            default:
                break;
        }
    }
}

template<typename T, typename U>
static void check_following_block_for_deleted(const std::vector<Entry> &oa, const T &o, std::vector<U> &deleted, uint32_t &start) {

    for (uint32_t j = start; j < oa.size(); ++j) {

        start = std::max(start, j);

        auto entry = oa[j];

        if (entry.m_type != Entry::SymbolEntry) {
            return;
        }

        deleted.push_back(o[j]);
    }
}

template<typename T, typename U>
std::unordered_map<std::string, std::vector<U>> HeckelDiff<T, U>::pass6(const T &n, const T &o, std::vector<Entry> &na, std::vector<Entry> &oa) {

    std::vector<U> inserted;
    std::vector<U> unchanged;
    std::vector<U> moved;
    std::vector<U> deleted;

    uint32_t i = 0;
    uint32_t j = 0;

    for (const auto &entry : na) {

        auto old_entry = oa[i];

        auto item = n[i];

        switch (entry.m_type) {

            case Entry::SymbolEntry: {

                inserted.push_back(item);

                j = std::max(i, j);

                break;
            }

            case Entry::LineNumber: {

                if (entry.m_line_number == old_entry.m_line_number) {

                    unchanged.push_back(item);

                } else {

                    moved.push_back(item);
                }

                check_following_block_for_deleted(oa, o, deleted, j+=1);

                break;
            }

            default:
                break;
        }

        i+=1;
    }

    return {
            {INSERTED, inserted},
            {DELETED, deleted},
            {MOVED, moved},
            {UNCHANGED, unchanged}
    };
}

template class HeckelDiff<std::string, char>;