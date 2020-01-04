#ifndef JESTER_RANDOM_PLAYER_HPP
#define JESTER_RANDOM_PLAYER_HPP

#include <random>

#include "../rules/player.hpp"
#include "../rules/game.hpp"

namespace jester {

class RandomPlayer : public Player {
private:
    std::random_device d_dev;
    std::mt19937 d_rng;

public:
    RandomPlayer();
    Action attack(const GameView& view, std::chrono::milliseconds time_limit);
    Action defend(const GameView & view, std::chrono::milliseconds time_limit);

private:
    Action randomCard(const std::vector<Card> & cards);
};

}

#endif
