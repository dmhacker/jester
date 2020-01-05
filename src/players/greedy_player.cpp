#include "greedy_player.hpp"
#include <iostream>

namespace jester {

GreedyPlayer::GreedyPlayer()
    : d_rng(d_dev())
{
}

Action GreedyPlayer::attack(const GameView& view, std::chrono::milliseconds time_limit)
{
    auto& hand = view.hand();
    if (view.currentAttack().empty()) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, hand.size() - 1);
        size_t idx = dist(d_rng);
        auto it = hand.begin();
        std::advance(it, idx);
        return Action(*it);
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

Action GreedyPlayer::defend(const GameView& view, std::chrono::milliseconds time_limit)
{
    auto& hand = view.hand();
    auto attacking = view.currentAttack().back();
    std::vector<Card> defenses;
    for (auto& card : hand) {
        if (attacking.suit() == view.trumpSuit()) {
            if (card.suit() == view.trumpSuit() && card.rank() > attacking.rank()) {
                defenses.push_back(card);
            }
        } else {
            if (card.suit() == view.trumpSuit()
                || (card.suit() == attacking.suit()
                    && card.rank() > attacking.rank())) {
                defenses.push_back(card);
            }
        }
    }
    return randomCard(defenses);
}

Action GreedyPlayer::randomCard(const std::vector<Card>& cards)
{
    if (cards.empty()) {
        return Action();
    }
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, cards.size() - 1);
    size_t idx = dist(d_rng);
    std::cerr << cards[idx] << std::endl;
    return Action(cards[idx]);
}

}