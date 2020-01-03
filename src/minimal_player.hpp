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
    std::shared_ptr<Card> attack(const GameView& view, bool firstAttack);
    std::shared_ptr<Card> defend(const GameView & view);
};

}

#endif
