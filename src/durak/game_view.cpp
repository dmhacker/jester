#include "game_view.hpp"
#include "game.hpp"

namespace jester {

GameView::GameView(const Game& game, size_t pid)
    : d_game(game)
    , d_pid(pid)
{
}

GameView::~GameView() {
}

Hand GameView::visibleHand(size_t pid) const
{
    Hand hand;
    auto& hidden = d_game.hiddenCards();
    for (auto& card : d_game.hand(pid)) {
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
        auto& hidden = d_game.hiddenCards();
        for (auto& card : d_game.hand(pid)) {
            if (hidden.find(card) != hidden.end()) {
                cnt++;
            }
        }
        return cnt;
    }
}

const Hand& GameView::hand() const
{
    return d_game.hand(d_pid);
}

size_t GameView::handSize(size_t pid) const
{
    return d_game.hand(pid).size();
}

bool GameView::finished() const
{
    return d_game.finished();
}

size_t GameView::deckSize() const
{
    return d_game.deckSize();
}

size_t GameView::playerCount() const 
{
    return d_game.playerCount();
}

const std::vector<size_t>& GameView::winOrder() const
{
    return d_game.winOrder();
}

const std::deque<size_t>& GameView::attackOrder() const
{
    return d_game.attackOrder();
}

const CardPile& GameView::hiddenCards() const
{
    return d_game.hiddenCards();
}

const CardSequence& GameView::currentAttack() const
{
    return d_game.currentAttack();
}

const CardSequence& GameView::currentDefense() const
{
    return d_game.currentDefense();
}

const Card& GameView::trumpCard() const
{
    return d_game.trumpCard();
}

Suit GameView::trumpSuit() const
{
    return d_game.trumpSuit();
}

size_t GameView::attackerId() const
{
    return d_game.attackerId();
}

size_t GameView::defenderId() const
{
    return d_game.defenderId();
}



}
