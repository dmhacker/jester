#ifndef JESTER_CFR_TABLE_HPP
#define JESTER_CFR_TABLE_HPP

#include "cfr_infoset.hpp"
#include "cfr_entry.hpp"

#include <memory>

namespace jester {

class CFRTable {
public:
    virtual std::unique_ptr<CFREntry> find(const CFRInfoSet&) = 0;
    virtual void save(const CFRInfoSet&, const CFREntry&) = 0;
    virtual size_t size() = 0;
};

}

#endif
