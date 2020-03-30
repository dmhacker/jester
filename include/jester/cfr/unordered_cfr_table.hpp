#ifndef JESTER_UNORDERED_CFR_TABLE_HPP
#define JESTER_UNORDERED_CFR_TABLE_HPP

#include "cfr_table.hpp"
#include "cfr_infoset.hpp"
#include "cfr_distribution.hpp"

#include <cereal/types/unordered_map.hpp>
#include <mutex>

namespace jester {

class UnorderedCFRTable : public CFRTable {
private:
    std::unordered_map<CFRInfoSet, CFRDistribution> d_strategy;
    std::unordered_map<CFRInfoSet, CFRDistribution> d_regrets;
    std::mutex d_smtx;
    std::mutex d_rmtx;

public:
    std::unique_ptr<CFRDistribution> findRegret(const CFRInfoSet&);
    std::unique_ptr<CFRDistribution> findProfile(const CFRInfoSet&, size_t num_actions);
    void saveRegret(const CFRInfoSet&, const CFRDistribution&);
    void incrementProfile(const CFRInfoSet&, size_t idx, size_t num_actions);
    size_t size();
};

}

#endif
