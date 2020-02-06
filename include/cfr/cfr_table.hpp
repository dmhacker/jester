#ifndef JESTER_CFR_TABLE_HPP
#define JESTER_CFR_TABLE_HPP

#include <memory>

namespace jester {

class CFRInfoSet;
class CFRDistribution;

class CFRTable {
protected:
    size_t d_hits;
    size_t d_misses;

public:
    virtual std::unique_ptr<CFRDistribution> findRegret(const CFRInfoSet&) = 0;
    virtual std::unique_ptr<CFRDistribution> findProfile(const CFRInfoSet&, size_t num_actions) = 0;
    virtual void saveRegret(const CFRInfoSet&, const CFRDistribution&) = 0;
    virtual void incrementProfile(const CFRInfoSet&, size_t idx, size_t num_actions) = 0;
    virtual size_t size() = 0;
    size_t hits() const;
    size_t misses() const;
};

inline size_t CFRTable::hits() const
{
    return d_hits;
}

inline size_t CFRTable::misses() const
{
    return d_misses;
}

}

#endif
