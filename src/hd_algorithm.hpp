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

namespace HeckelDiff {

    static const uint32_t NotFound = std::numeric_limits<uint32_t>::max();

    struct Entry final {

        std::stack<uint32_t> all_old_indexes;

        uint32_t oc = 0;
        uint32_t nc = 0;
    };

    template<typename T>
    struct Record final {

    private:
        uint32_t m_index = NotFound;

    public:
        enum Type {
            SymbolTableEntry,
            LineNumber
        };

        Entry *entry = nullptr;

        T value;
        Type type = SymbolTableEntry;


        Record<T>(Entry *entry, const T value) : entry(entry), value(value) {
            type = SymbolTableEntry;
        }

        void set_index(const uint32_t index) {
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

    private:

        std::unordered_map<T, std::unique_ptr<Entry>> symbol_table;
        std::vector<Record<T>> oa;
        std::vector<Record<T>> na;

        void pass1(const std::vector<T> &n, std::unordered_map<T, std::unique_ptr<Entry>> &symbolTable, std::vector<Record<T>> &na);

        void pass2(const std::vector<T> &o, std::unordered_map<T, std::unique_ptr<Entry>>&symbolTable, std::vector<Record<T>> &oa);

        void pass3(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        void pass4(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        void pass5(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

        std::unordered_map<std::string, std::vector<T>> pass6(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

    public:

        const std::string INSERTED = "inserted";
        const std::string DELETED = "deleted";
        const std::string MOVED = "moved";
        const std::string UNCHANGED = "unchanged";

        auto diff(const std::vector<T> original, const std::vector<T> updated) {

            oa.reserve(original.size());
            na.reserve(updated.size());

            pass1(updated, symbol_table, na);
            pass2(original, symbol_table, oa);
            pass3(na, oa);
            pass4(na, oa);
            pass5(na, oa);

            auto result = pass6(na, oa);

            return result;
        }
    };
}

#endif //${name}
