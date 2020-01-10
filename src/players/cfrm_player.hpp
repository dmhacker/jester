#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/cfrm_learner.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    CFRMLearner d_learner;
    std::mt19937 d_rng;

public:
    CFRMPlayer(size_t iterations);
    Action nextAction(const GameView& view);
};

}

#endif
