#ifndef HeckelDiff_H
#define HeckelDiff_H

#include <string>
#include <unordered_map>

struct Entry;
class SymbolTableEntry;

class HeckelDiff {

    ~HeckelDiff() {

        for (auto &item : symbolTable) {
            delete item;
        }

        symbolTable.clear();
    }

private:
    void pass1(const std::string &n);
    void pass2(const std::string &o);
    void pass3(const std::string &o);
    void pass4(const std::string &o);

public:
    std::unordered_map<char, SymbolTableEntry*> symbolTable;
    std::vector<Entry> oa;
    std::vector<Entry> na;

    bool diff(std::string original, std::string updated);
};


#endif //${name}
