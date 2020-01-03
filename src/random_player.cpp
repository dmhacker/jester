#include "random_player.hpp"

#include <cassert>

namespace jester {

RandomPlayer::RandomPlayer()
    : d_rng(d_dev())
{
}

std::shared_ptr<Card> RandomPlayer::attack(const GameView& view, bool firstAttack)
{
    auto& hand = view.hand();
    if (firstAttack) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, hand.size() - 1);
        size_t idx = dist(d_rng);
        auto it = hand.begin();
        std::advance(it, idx);
        return std::make_shared<Card>(*it);
    } else {
        std::unordered_set<size_t> valid_ranks;
        for (auto& card : view.currentAttack()) {
            valid_ranks.insert(card.rank());
        }
        for (auto& card : view.currentDefense()) {
            valid_ranks.insert(card.rank());
        }
        std::vector<Card> attacks;
        for (auto& card : hand) {
            if (valid_ranks.find(card.rank()) != valid_ranks.end()) {
                attacks.push_back(card);
            }
        }
        return randomCard(attacks);
    }
}

std::shared_ptr<Card> RandomPlayer::defend(const GameView& view)
{
    assert(view.currentAttack().size() == view.currentDefense().size() + 1);
    auto& hand = view.hand();
    auto attacking = view.currentAttack().back();
    std::vector<Card> defenses;
    for (auto& card : hand) {
        if (attacking.suit() == view.trumpSuit()) {
            if (card.suit() == view.trumpSuit() && card.rank() > attacking.rank()) {
                defenses.push_back(card);
            }
        } else {
            if (card.suit() == view.trumpSuit() || card.rank() > attacking.rank()) {
                defenses.push_back(card);
            }
        }
    }
    return randomCard(defenses);
}

std::shared_ptr<Card> RandomPlayer::randomCard(const std::vector<Card>& cards)
{
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, cards.size());
    size_t idx = dist(d_rng);
    if (idx < cards.size()) {
        return std::make_shared<Card>(cards[idx]);
    }
    return nullptr;
}

}
