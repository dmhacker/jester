#ifndef JESTER_CFR_TABLE_HPP
#define JESTER_CFR_TABLE_HPP

#include "cfr_infoset.hpp"
#include "cfr_entry.hpp"

#include <memory>

namespace jester {

class CFRTable {
private:
    size_t d_hits;
    size_t d_misses;

public:
    virtual std::unique_ptr<CFREntry> find(const CFRInfoSet&) = 0;
    virtual void save(const CFRInfoSet&, const CFREntry&) = 0;
    virtual size_t size() = 0;
    size_t hits() const;
    size_t misses() const;

protected:
    void onFind(bool hit);
};

inline size_t CFRTable::hits() const {
    return d_hits;
}

inline size_t CFRTable::misses() const {
    return d_misses;
}

inline void CFRTable::onFind(bool hit) {
    if (hit) {
        d_hits++;
    }
    else {
        d_misses++;
    }
}

}

#endif
