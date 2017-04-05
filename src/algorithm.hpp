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

class Entry final {

public:
    enum Type: char {
        SymbolEntry,
        LineNumber
    };

//    const Type &type = m_type;
//    const uint32_t &line_number = m_line_number;
//    const SymbolTableEntry *&symbol_table_entry = m_symbol_table_entry;

    Entry(const SymbolTableEntry *symbol_table_entry, char debug) : m_symbol_table_entry(symbol_table_entry), m_debug(debug) {
        m_type = SymbolEntry;
    }

    void set_line_number(const uint32_t line_number) {
        m_line_number = line_number;
        m_type = LineNumber;
    }

//private:
    Type m_type = LineNumber;
    uint32_t m_line_number = 0;
    const SymbolTableEntry *m_symbol_table_entry = nullptr;
    char m_debug = ' ';
};

template<typename T, typename U>
class HeckelDiff {

private:
    std::unordered_map<U, SymbolTableEntry*> symbol_table;
    std::vector<Entry> oa;
    std::vector<Entry> na;

    void pass1(const T &n, std::unordered_map<U, SymbolTableEntry*> &symbolTable, std::vector<Entry> &na);
    void pass2(const T &o, std::unordered_map<U, SymbolTableEntry*> &symbolTable, std::vector<Entry> &oa);
    void pass3(std::vector<Entry> &na, std::vector<Entry> &oa);
    void pass4(std::vector<Entry> &na, std::vector<Entry> &oa);
    void pass5(std::vector<Entry> &na, std::vector<Entry> &oa);
    std::unordered_map<std::string, std::vector<U>> pass6(const T &n, const T &o, std::vector<Entry> &na, std::vector<Entry> &oa);

public:

    ~HeckelDiff() {

        for (auto item : symbol_table) {
            delete item.second;
        }

        symbol_table.clear();
    }

    std::unordered_map<std::string, std::vector<U>> diff(const T original, const T updated) {

        oa.reserve(original.length());
        na.reserve(updated.length());
        symbol_table.reserve(original.length() + updated.length());

        pass1(updated, symbol_table, na);
        pass2(original, symbol_table, oa);
        pass3(na, oa);
        pass4(na, oa);
        pass5(na, oa);

        return pass6(updated, original, na, oa);
    }
};

#endif //${name}
