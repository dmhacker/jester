#include "minimal_player.hpp"

namespace jester {

MinimalPlayer::MinimalPlayer()
    : d_rng(std::random_device{}())
{
}

Action MinimalPlayer::nextAction(const GameView& view)
{
    auto& hand = view.hand();
    if (view.attackerNext() && view.currentAttack().empty()) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, hand.size() - 1);
        size_t idx = dist(d_rng);
        auto it = hand.begin();
        std::advance(it, idx);
        return Action(*it);
    } else {
        return Action(); 
    }
}

}
