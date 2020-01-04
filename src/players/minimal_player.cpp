#include "minimal_player.hpp"

namespace jester {

MinimalPlayer::MinimalPlayer()
    : d_rng(d_dev())
{
}

Action MinimalPlayer::attack(const GameView& view, std::chrono::milliseconds time_limit)
{
    auto& hand = view.hand();
    if (view.currentAttack().empty()) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, hand.size() - 1);
        size_t idx = dist(d_rng);
        auto it = hand.begin();
        std::advance(it, idx);
        return Action(*it);
    } else {
        return Action(); 
    }
}

Action MinimalPlayer::defend(const GameView& view, std::chrono::milliseconds time_limit)
{
    return Action(); 
}

}