#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/tabular_cfrm.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    TabularCFRM d_cfrm;

public:
    Action nextAction(const GameView& view);
};

}

#endif
