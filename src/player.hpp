#ifndef JESTER_PLAYER_HPP
#define JESTER_PLAYER_HPP

#include <memory>

#include "card.hpp"

namespace jester {

class GameView;

class Player {
public:
    virtual std::shared_ptr<Card> attack(const GameView& view, bool firstAttack) = 0;
    virtual std::shared_ptr<Card> defend(const GameView & view) = 0;
};
}

#endif
