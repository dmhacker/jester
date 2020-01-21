#ifndef JESTER_HUMAN_PLAYER_HPP
#define JESTER_HUMAN_PLAYER_HPP

#include "player.hpp"

namespace jester {

class HumanPlayer : public Player {
public:
    Action nextAction(const GameView& view);
};

}

#endif
