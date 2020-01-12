#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/cfrm_environment.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    CFRMEnvironment d_cfrm;
    bool d_verbose;

public:
    CFRMPlayer(bool verbose);

    Action nextAction(const GameView& view);
};

}

#endif
