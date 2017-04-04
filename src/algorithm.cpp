#include "algorithm.hpp"

#include <vector>
#include <unordered_map>

struct Entry {

    enum Type: char {
        SymbolTableEntry = 0,
        LineNumber
    };

private:
    Type m_entry_type;
    const SymbolTableEntry *m_symbolTableEntry; // either the lines SymbolTableEntry OR line number in the "file" N
    uint32_t m_lineNumber = 0;

public:
    const Type &entry_type = m_entry_type;
    const SymbolTableEntry *&symbolTableEntry = m_symbolTableEntry;
    const uint32_t &lineNumber = m_lineNumber;

    void updateEntryToSymbolTable(const SymbolTableEntry *&symbolTableEntry) {
        m_symbolTableEntry = symbolTableEntry;
        m_entry_type = SymbolTableEntry;
    }

    void updateEntryToLineNumber(const uint32_t lineNumber) {
        m_lineNumber = lineNumber;
        m_entry_type = LineNumber;
    }
};

class SymbolTableEntry final {

private:
    uint32_t ocCount = 0;
    uint32_t ncCount = 0;

    Counter m_oc = Zero;
    Counter m_nc = Zero;
    uint32_t m_olno = 0;

public:
    enum Counter: char {
        Zero = 0,
        One,
        Many
    };

    const Counter &oc = m_oc;
    const Counter &nc = m_nc;

    const uint32_t &olno = m_olno; // of interest only if OC=1.

    void ncIncrement() {

        switch (++ncCount) {
            case 0:
                m_nc = Zero;
            case 1:
                m_nc = One;
            default:
                m_nc = Many;
        }
    }

    void ocIncrement() {

        switch (++ocCount) {
            case 0:
                m_oc = Zero;
            case 1:
                m_oc = One;
            default:
                m_oc = Many;
        }
    }

    void updateOLNO(uint32_t number) {
        m_olno = number;
    }
};

// TODO: how to handle duplicate "lines"
void HeckelDiff::pass1(const std::string &n) {

    for (uint32_t i = 0; i < n.length(); ++i) {

        auto item = n[i];

        if (symbolTable.find(item) == symbolTable.end()) {

            auto entry = new SymbolTableEntry();
            symbolTable[item] = entry;
        }

        symbolTable[item]->ncIncrement();

        na[i].updateEntryToSymbolTable(*symbolTable[item]);
    }
}

void HeckelDiff::pass2(const std::string &o) {

    for (uint32_t i = 0; i < o.length(); ++i) {

        auto item = o[i];

        if (symbolTable.find(item) == symbolTable.end()) {

            auto entry = new SymbolTableEntry();
            symbolTable[item] = entry;
        }

        symbolTable[item]->ocIncrement();

        oa[i].updateEntryToSymbolTable(*symbolTable[item]);

        symbolTable[item]->updateOLNO(i);
    }
}

//A line that occurs once and only once in each file must be the same line (unchanged but possibly moved).
void HeckelDiff::pass3(const std::string &n) {

    for (uint32_t i = 0; i < n.size(); ++i) {

        auto symbol = symbolTable[n[i]];

        if (symbol->nc == symbol->oc == SymbolTableEntry::One) {

            symbol->updateOLNO(i);
            na[i].updateEntryToLineNumber(symbol->olno);
        }
    }

    // TODO: "Find" unique virtual lines immediately before the first and immediately after the last lines of the files????
}

// If a line has been found to be unaltered, and the lines immediately adjacent to it in both files are identical, then these lines must be the same line. This information can be used to find blocks of unchanged lines.
void HeckelDiff::pass4(const std::string &n) {

}

bool HeckelDiff::diff(std::string o, std::string n) {

    pass1(n);
    pass2(o);
    pass3(n);

    return true;
}