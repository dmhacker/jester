#ifndef JESTER_CFR_PLAYER_HPP
#define JESTER_CFR_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfr/cfr_engine.hpp"

namespace jester {

class CFRPlayer : public Player {
private:
    CFREngine d_engine;
    std::mt19937 d_rng;

public:
    CFRPlayer();
    Action nextAction(const GameView& view);
};

}

#endif
