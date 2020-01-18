#include "game_view.hpp"
#include "game_state.hpp"

namespace jester {

GameView::GameView(const GameState& state, size_t pid)
    : d_state(state)
    , d_pid(pid)
{
}

Hand GameView::visibleHand(size_t pid) const
{
    Hand hand;
    auto& hidden = d_state.hiddenCards();
    for (auto& card : d_state.hand(pid)) {
        if (d_pid == pid || hidden.find(card) == hidden.end()) {
            hand.insert(card);
        }
    }
    return hand;
}

size_t GameView::hiddenHandSize(size_t pid) const
{
    if (d_pid == pid) {
        return 0;
    } else {
        size_t cnt = 0;
        auto& hidden = d_state.hiddenCards();
        for (auto& card : d_state.hand(pid)) {
            if (hidden.find(card) != hidden.end()) {
                cnt++;
            }
        }
        return cnt;
    }
}

const Hand& GameView::hand() const
{
    return d_state.hand(d_pid);
}

size_t GameView::handSize(size_t pid) const
{
    return d_state.hand(pid).size();
}

size_t GameView::turn() const
{
    return d_state.turn();
}

bool GameView::finished() const
{
    return d_state.finished();
}

bool GameView::attackerNext() const
{
    return d_state.attackerNext();
}

size_t GameView::deckSize() const
{
    return d_state.deck().size();
}

size_t GameView::playerCount() const
{
    return d_state.playerCount();
}

const std::vector<Action>& GameView::nextActions() const
{
    return d_state.nextActions();
}

const std::vector<size_t>& GameView::winOrder() const
{
    return d_state.winOrder();
}

const std::deque<size_t>& GameView::attackOrder() const
{
    return d_state.attackOrder();
}

const CardPile& GameView::hiddenCards() const
{
    return d_state.hiddenCards();
}

const CardSequence& GameView::currentAttack() const
{
    return d_state.currentAttack();
}

const CardSequence& GameView::currentDefense() const
{
    return d_state.currentDefense();
}

const Card& GameView::trumpCard() const
{
    return d_state.trumpCard();
}

Suit GameView::trumpSuit() const
{
    return d_state.trumpSuit();
}

size_t GameView::attackerId() const
{
    return d_state.attackerId();
}

size_t GameView::defenderId() const
{
    return d_state.defenderId();
}

}
