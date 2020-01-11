#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/cfrm_storage.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    CFRMStorage d_cfrm;

public:
    Action nextAction(const GameView& view);
};

}

#endif
