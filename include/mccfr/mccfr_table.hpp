#ifndef JESTER_MCCFR_TABLE_HPP
#define JESTER_MCCFR_TABLE_HPP

#include "mccfr_entry.hpp"
#include "mccfr_infoset.hpp"

#include <mutex>
#include <random>
#include <redox.hpp>
#include <thread>

#include <cereal/types/unordered_map.hpp>

namespace jester {

class GameState;

class MCCFRTable {
private:
    redox::Redox& d_rdx;

public:
    MCCFRTable(redox::Redox& redox);
    Action bestAction(const GameView& view, std::mt19937& rng);
    Action sampleAction(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng);
    float train(size_t tpid, const GameState& state, std::mt19937& rng);

private:
    std::string toString(const MCCFRInfoSet& infoset);
    std::string toString(const MCCFREntry& entry);
    MCCFREntry fromString(const std::string& strn);
};

}

#endif
