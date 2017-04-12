/*
 * Copyright 2017 Rowun Giles - http://github.com/rowungiles
 */

#ifndef HeckelDiff_H
#define HeckelDiff_H

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <limits>
#include <memory>
#include <functional>

namespace HeckelDiff {

    static const std::string INSERTED = "inserted";
    static const std::string DELETED = "deleted";
    static const std::string MOVED = "moved";
    static const std::string UNCHANGED = "unchanged";

    static const size_t NotFound = std::numeric_limits<size_t>::max();

    struct Entry final {

        std::stack<size_t> all_old_indexes;

        size_t oc = 0;
        size_t nc = 0;
    };

    template<typename T>
    struct Record final {

    private:
        size_t m_index = NotFound;

    public:
        enum Type {
            SymbolTableEntry,
            LineNumber
        };

        T value {};
        Entry *entry = nullptr;
        Type type = SymbolTableEntry;


        Record<T>() {}

        Record<T>(const T value, Entry *entry) : value(value), entry(entry) {
            type = SymbolTableEntry;
        }

        void set_index(const size_t index) {
            m_index = index;
            type = LineNumber;
        }

        auto index() const {
            return m_index;
        }

        bool operator==(const Record<T> &rhs) const {
            return this->type == rhs.type && this->entry == rhs.entry;
        }

        bool operator!=(const Record<T> &rhs) const {
            return this->type != rhs.type || this->entry != rhs.entry;
        }
    };

    template<typename T>
    class Algorithm {

        enum Direction {
            Ascending = 1,
            Descending = - 1
        };

        std::unordered_map<T, std::unique_ptr<Entry>> symbol_table;
        std::vector<Record<T>> oa;
        std::vector<Record<T>> na;

        static void index_item(const T &item, std::unordered_map<T, std::unique_ptr<Entry>> &symbol_table, std::function<void(Entry *&entry, const T &l_item)> &lambda);
        static void find_unchanged_blocks(const Record<T> &record, const Direction &direction, const size_t &i, std::vector<Record<T>> &na, std::vector<Record<T>> &oa);
        static std::vector<T> populate_deleted_items(const std::vector<Record<T>> &oa);
        static auto populate_new_items(const std::vector<Record<T>> &na, const std::vector<Record<T>> &oa);

        static void pass1(const std::vector<T> &n, std::unordered_map<T, std::unique_ptr<Entry>> &symbolTable, std::vector<Record<T>> &na);

        static void pass2(const std::vector<T> &o, std::unordered_map<T, std::unique_ptr<Entry>>&symbolTable, std::vector<Record<T>> &oa);

        static void pass3(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        static void pass4(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        static void pass5(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        static const std::unordered_map<std::string, std::vector<T>> pass6(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

    public:
        auto diff(const std::vector<T> original, const std::vector<T> updated) {

            oa.resize(original.size());
            na.resize(updated.size());

            pass1(updated, symbol_table, na);
            pass2(original, symbol_table, oa);
            pass3(na, oa);
            pass4(na, oa);
            pass5(na, oa);

            auto result = pass6(na, oa);

            symbol_table.clear();
            oa.clear();
            na.clear();

            return result;
        }
    };
}

#endif //${name}
