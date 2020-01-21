#ifndef JESTER_UNORDERED_CFR_TABLE_HPP
#define JESTER_UNORDERED_CFR_TABLE_HPP

#include "cfr_table.hpp"

#include <cereal/types/unordered_map.hpp>

namespace jester {

class UnorderedCFRTable : public CFRTable {
private:
    std::unordered_map<CFRInfoSet, CFREntry> d_map;

public:
    std::unique_ptr<CFREntry> find(const CFRInfoSet&);
    void save(const CFRInfoSet&, const CFREntry&);
    size_t size();
};

}

#endif
