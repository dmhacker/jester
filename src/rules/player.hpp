#ifndef JESTER_PLAYER_HPP
#define JESTER_PLAYER_HPP

#include <memory>

#include "card.hpp"

namespace jester {

class GameView;

class Player {
public:
    virtual Action attack(const GameView& view) = 0;
    virtual Action defend(const GameView & view) = 0;
};
}

#endif
