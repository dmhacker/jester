#ifndef JESTER_MCCFR_PLAYER_HPP
#define JESTER_MCCFR_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../mccfr/mccfr_engine.hpp"

namespace jester {

class MCCFRPlayer : public Player {
private:
    MCCFREngine d_engine;
    std::mt19937 d_rng;

public:
    MCCFRPlayer();
    Action nextAction(const GameView& view);
};

}

#endif
