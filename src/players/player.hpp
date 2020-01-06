#ifndef JESTER_PLAYER_HPP
#define JESTER_PLAYER_HPP

#include <chrono>

#include "../rules/game_view.hpp"

namespace jester {

class Player {
public:
    virtual Action nextAction(const GameView& view) = 0;
};

}

#endif
