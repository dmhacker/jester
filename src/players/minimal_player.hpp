#ifndef JESTER_MINIMAL_PLAYER_HPP
#define JESTER_MINIMAL_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class MinimalPlayer : public Player {
private:
    std::random_device d_dev;
    std::mt19937 d_rng;

public:
    MinimalPlayer();
    Action nextAction(const GameView & view);
};

}

#endif
