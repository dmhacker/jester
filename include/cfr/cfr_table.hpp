#ifndef JESTER_CFR_TABLE_HPP
#define JESTER_CFR_TABLE_HPP

#include "cfr_entry.hpp"
#include "cfr_infoset.hpp"
#include "../game/card.hpp"

#include <mutex>
#include <random>
#include <redox.hpp>
#include <thread>

#include <cereal/types/unordered_map.hpp>

namespace jester {

class GameState;

class CFRTable {
private:
    redox::Redox& d_rdx;

public:
    CFRTable(redox::Redox& redox);
    Action bestAction(const GameView& view, std::mt19937& rng);
    int train(size_t tpid, const GameState& state, std::mt19937& rng);

private:
    size_t sampleIndex(const std::vector<float>& profile, std::mt19937& rng) const;
    std::unique_ptr<CFREntry> findEntry(const CFRInfoSet& infoset);
    void saveEntry(const CFRInfoSet& infoset, const CFREntry& entry);
};

}

#endif
