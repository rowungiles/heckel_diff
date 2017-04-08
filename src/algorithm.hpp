#ifndef HeckelDiff_H
#define HeckelDiff_H

#include <string>
#include <unordered_map>
#include <vector>

class SymbolTableEntry final {

public:
    enum Counter: char {
        Zero = 0,
        One,
        Many
    };

//    const Counter &oc = m_oc;
//    const Counter &nc = m_nc;
//    const uint32_t &olno = m_olno;  // of interest only if OC=One.

    void increment(Counter &counter) {
        switch (counter) {
            case Zero:
                counter = One;
                break;
            default:
                counter = Many;
        }
    }
    void nc_increment() {
        increment(m_nc);
    }

    void oc_increment() {
        increment(m_oc);
    }

    void update_olno(uint32_t number) {
        m_olno = number;
    }

    bool operator==(const SymbolTableEntry& rhs) {
        return this->m_nc == rhs.m_nc && this->m_oc == rhs.m_oc && this->m_olno == rhs.m_olno;
    }

//private:
    Counter m_oc = Zero;
    Counter m_nc = Zero;
    uint32_t m_olno = 0;

};

template<typename T>
class Entry final {

public:
    enum Type: char {
        SymbolEntry,
        LineNumber
    };

//    const Type &type = m_type;
//    const uint32_t &line_number = m_line_number;
//    const SymbolTableEntry *&symbol_table_entry = m_symbol_table_entry;

    Entry<T>(const SymbolTableEntry *symbol_table_entry, T value) : m_symbol_table_entry(symbol_table_entry), m_value(value) {
        m_type = SymbolEntry;
    }

    void set_line_number(const uint32_t line_number) {
        m_line_number = line_number;
        m_type = LineNumber;
    }

    bool operator==(const Entry<T>& rhs) {
        return this->m_type == rhs.m_type && this->m_symbol_table_entry == rhs.m_symbol_table_entry;
    }

    bool operator!=(const Entry<T>& rhs) {
        return this->m_type != rhs.m_type || this->m_symbol_table_entry != rhs.m_symbol_table_entry;
    }

//private:
    Type m_type = LineNumber;
    uint32_t m_line_number = 0;
    const SymbolTableEntry *m_symbol_table_entry = nullptr;
    T m_value;
};

template<typename T>
class HeckelDiff {

private:
    std::unordered_map<T, SymbolTableEntry*> symbol_table;
    std::vector<Entry<T>> oa;
    std::vector<Entry<T>> na;

    void pass1(const std::vector<T> &n, std::unordered_map<T, SymbolTableEntry*> &symbolTable, std::vector<Entry<T>> &na);
    void pass2(const std::vector<T> &o, std::unordered_map<T, SymbolTableEntry*> &symbolTable, std::vector<Entry<T>> &oa);
    void pass3(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa);
    void pass4(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa);
    void pass5(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa);
    std::unordered_map<std::string, std::vector<T>> pass6(std::vector<Entry<T>> &na, std::vector<Entry<T>> &oa);

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
