#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/cfrm_environment.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    CFRMEnvironment d_cfrm;
    std::mt19937 d_rng;

public:
    CFRMPlayer(bool verbose);

    Action nextAction(const GameView& view);
};

}

#endif
