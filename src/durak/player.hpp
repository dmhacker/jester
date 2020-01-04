#ifndef JESTER_PLAYER_HPP
#define JESTER_PLAYER_HPP

#include <chrono>

#include "card.hpp"
#include "game_view.hpp"

namespace jester {

class GameView;

class Player {
public:
    virtual Action attack(const GameView& view, std::chrono::milliseconds time_limit) = 0;
    virtual Action defend(const GameView & view, std::chrono::milliseconds time_limit) = 0;
};
}

#endif
