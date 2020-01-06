#ifndef JESTER_DMCTS_PLAYER_HPP
#define JESTER_DMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class DMCTSPlayer : public Player {
public:
    Action nextAction(const GameView& view);
};

}

#endif
