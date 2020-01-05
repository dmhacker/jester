#ifndef JESTER_GREEDY_PLAYER_HPP
#define JESTER_GREEDY_PLAYER_HPP

#include "random_player.hpp"

namespace jester {

class GreedyPlayer : public RandomPlayer {
private:
    Action randomAction(const std::vector<Card>& cards);
};

}

#endif
