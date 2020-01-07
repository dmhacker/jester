#ifndef JESTER_RANDOM_PLAYER_HPP
#define JESTER_RANDOM_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class RandomPlayer : public Player {
private:
    std::mt19937 d_rng;

public:
    RandomPlayer();
    Action nextAction(const GameView& view);
};

}

#endif
