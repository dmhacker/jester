#ifndef JESTER_RANDOM_PLAYER_HPP
#define JESTER_RANDOM_PLAYER_HPP

#include <memory>
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
    std::shared_ptr<Card> attack(const GameView& view, bool firstAttack);
    std::shared_ptr<Card> defend(const GameView & view);
private:
    std::shared_ptr<Card> randomCard(const std::vector<Card> & cards);
};
}

#endif
