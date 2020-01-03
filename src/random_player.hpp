#ifndef JESTER_RANDOM_PLAYER_HPP
#define JESTER_RANDOM_PLAYER_HPP

#include <memory>

#include "player.hpp"
#include "game.hpp"

namespace jester {

class RandomPlayer {
public:
    AttackSequence attack(const GameView& view, bool firstAttack);
    DefenseSequence defend(const GameView & view);
};
}

#endif
