#include "minimal_player.hpp"

namespace jester {

MinimalPlayer::MinimalPlayer()
    : d_rng(d_dev())
{
}

std::shared_ptr<Card> MinimalPlayer::attack(const GameView& view, bool firstAttack)
{
    auto& hand = view.hand();
    if (firstAttack) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, hand.size() - 1);
        size_t idx = dist(d_rng);
        auto it = hand.begin();
        std::advance(it, idx);
        return std::make_shared<Card>(*it);
    } else {
        return nullptr; 
    }
}

std::shared_ptr<Card> MinimalPlayer::defend(const GameView& view)
{
    return nullptr; 
}

}
