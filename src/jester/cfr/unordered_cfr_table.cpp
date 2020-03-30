#include <jester/cfr/unordered_cfr_table.hpp>
#include <jester/game/game_state.hpp>
#include <jester/game/game_view.hpp>

#include <algorithm>

namespace jester {

std::unique_ptr<CFRDistribution> UnorderedCFRTable::findRegret(const CFRInfoSet& infoset)
{
    std::lock_guard<std::mutex> lck(d_rmtx);
    auto it = d_regrets.find(infoset);
    if (it != d_regrets.end()) {
        d_hits++;
        return std::unique_ptr<CFRDistribution>(new CFRDistribution(it->second));
    } else {
        d_misses++;
        return nullptr;
    }
}

std::unique_ptr<CFRDistribution> UnorderedCFRTable::findProfile(const CFRInfoSet& infoset, size_t num_actions)
{
    std::lock_guard<std::mutex> lck(d_smtx);
    auto it = d_strategy.find(infoset);
    if (it != d_strategy.end()) {
        d_hits++;
        return std::unique_ptr<CFRDistribution>(new CFRDistribution(it->second));
    } else {
        d_misses++;
        return nullptr;
    }
}

void UnorderedCFRTable::saveRegret(const CFRInfoSet& infoset,
    const CFRDistribution& distribution)
{
    std::lock_guard<std::mutex> lck(d_rmtx);
    d_regrets[infoset] = distribution;
}

void UnorderedCFRTable::incrementProfile(const CFRInfoSet& infoset, size_t idx, size_t num_actions)
{
    std::lock_guard<std::mutex> lck(d_smtx);
    auto it = d_strategy.find(infoset);
    if (it == d_strategy.end()) {
        CFRDistribution distribution(num_actions);
        distribution.add(idx, 1);
        d_strategy[infoset] = distribution;
    }
    else {
        it->second.add(idx, 1);
    }
}

size_t UnorderedCFRTable::size()
{
    std::lock_guard<std::mutex> lck(d_smtx);
    return d_strategy.size();
}

}
