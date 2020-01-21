#ifndef JESTER_CFR_PLAYER_HPP
#define JESTER_CFR_PLAYER_HPP

#include <random>

#include "../cfr/cfr_engine.hpp"
#include "player.hpp"

namespace jester {

class CFRPlayer : public Player {
private:
    CFREngine& d_cfr;
    std::mt19937 d_rng;

public:
    CFRPlayer(CFREngine& cfr);
    Action nextAction(const GameView& view);
};

}

#endif
