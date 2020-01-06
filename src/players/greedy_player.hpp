#ifndef JESTER_GREEDY_PLAYER_HPP
#define JESTER_GREEDY_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class GreedyPlayer : public Player {
private:
    std::random_device d_dev;
    std::mt19937 d_rng;

public:
    GreedyPlayer();
    Action attack(const GameView& view, std::chrono::milliseconds time_limit);
    Action defend(const GameView& view, std::chrono::milliseconds time_limit);
};

}

#endif
