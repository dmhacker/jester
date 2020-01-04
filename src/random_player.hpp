#ifndef JESTER_RANDOM_PLAYER_HPP
#define JESTER_RANDOM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "game.hpp"

namespace jester {

class RandomPlayer : public Player {
private:
    std::random_device d_dev;
    std::mt19937 d_rng;

public:
    RandomPlayer();
    Action attack(const GameView& view);
    Action defend(const GameView & view);

private:
    Action randomCard(const std::vector<Card> & cards);
};

}

#endif
