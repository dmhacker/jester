#ifndef JESTER_MINIMAL_PLAYER_HPP
#define JESTER_MINIMAL_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "game.hpp"

namespace jester {

class MinimalPlayer : public Player {
private:
    std::random_device d_dev;
    std::mt19937 d_rng;

public:
    MinimalPlayer();
    Action attack(const GameView& view);
    Action defend(const GameView & view);
};

}

#endif
