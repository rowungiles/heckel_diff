#ifndef HeckelDiff_H
#define HeckelDiff_H

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <limits>

static const uintmax_t NotFound = UINT_MAX;

class Entry final {

private:
    std::stack<uint32_t> m_all_olno;

    uint32_t m_oc = 0;
    uint32_t m_nc = 0;

public:
    Entry() {
        push_olno(NotFound);
    }

    void nc_increment() {
        m_nc += 1;
    }

    void oc_increment() {
        m_oc += 1;
    }

    void oc_decrement() {
        m_oc -= 1;
    }

    void pop_olno() {
        m_all_olno.pop();
    };

    void push_olno(uint32_t index) {
        m_all_olno.push(index);
    }

    auto olno() const {
        return m_all_olno.top();
    };

    auto oc() const {
        return m_oc;
    }

    auto nc() const {
        return m_nc;
    }
};

template<typename T>
class Record final {

public:
    enum Type {
        SymbolTableEntry,
        LineNumber
    };

    class Entry *entry = nullptr;

private:
    T m_value;
    Type m_type = SymbolTableEntry;
    uint32_t m_index = NotFound;

public:
    Record<T>(class Entry *entry, T value) : entry(entry), m_value(value) {
        m_type = SymbolTableEntry;
    }

    bool operator==(const Record<T>& rhs) {
        return this->m_type == rhs.m_type && this->entry == rhs.entry;
    }

    bool operator!=(const Record<T>& rhs) {
        return this->m_type != rhs.m_type || this->entry != rhs.entry;
    }

    void set_index(const uint32_t index) {
        m_index = index;
        m_type = LineNumber;
    }

    auto type() const {
        return m_type;
    }

    auto value() const {
        return m_value;
    }

    auto index() const {
        return m_index;
    }
};

template<typename T>
class HeckelDiff {

private:
    std::unordered_map<T, Entry*> symbol_table;
    std::vector<Record<T>> oa;
    std::vector<Record<T>> na;

    void pass1(const std::vector<T> &n, std::unordered_map<T, Entry*> &symbolTable, std::vector<Record<T>> &na);
    void pass2(const std::vector<T> &o, std::unordered_map<T, Entry*> &symbolTable, std::vector<Record<T>> &oa);
    void pass3(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);
    void pass4(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);
    void pass5(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);
    std::unordered_map<std::string, std::vector<T>> pass6(std::vector<Record<T>> &na, std::vector<Record<T>> &oa);

public:

    ~HeckelDiff() {

        for (auto item : symbol_table) {
            delete item.second;
        }

        symbol_table.clear();
    }

    std::unordered_map<std::string, std::vector<T>> diff(const std::vector<T> original, const std::vector<T> updated) {

        oa.reserve(original.size());
        na.reserve(updated.size());
        symbol_table.reserve(original.size() + updated.size());

        pass1(updated, symbol_table, na);
        pass2(original, symbol_table, oa);
        pass3(na, oa);
        pass4(na, oa);
        pass5(na, oa);

        return pass6(na, oa);
    }
};

#endif //${name}
