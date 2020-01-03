#ifndef JESTER_PLAYER_HPP
#define JESTER_PLAYER_HPP

#include <memory>

#include "card.hpp"

namespace jester {

class GameView;

class Player {
public:
    virtual AttackSequence attack(const GameView& view, bool firstAttack) = 0;
    virtual DefenseSequence defend(const GameView & view) = 0;
};
}

#endif
