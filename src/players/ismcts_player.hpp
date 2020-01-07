#ifndef JESTER_ISMCTS_PLAYER_HPP
#define JESTER_ISMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class ISMCTSPlayer : public Player {
public:
    Action nextAction(const GameView& view);
};

}

#endif
