#include "greedy_player.hpp"

namespace jester {

Action GreedyPlayer::randomAction(const std::vector<Card>& cards)
{
    // Only select an empty action if nothing else is possible
    if (cards.empty()) {
        return Action();
    }
    // If there are available cards, pick one at random
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, cards.size() - 1);
    size_t idx = dist(d_rng);
    return Action(cards[idx]);
}

}
