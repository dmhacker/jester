#ifndef JESTER_GAME_VIEW_HPP
#define JESTER_GAME_VIEW_HPP

#include <memory>

#include "card.hpp"

namespace jester {

class Game;

class GameView {
private:
    const Game& d_game;
    size_t d_pid;

public:
    GameView(const Game& game, size_t pid);
    ~GameView();

    // Information relative to the owning player
    size_t playerId() const;
    const Hand& hand() const;
    Hand visibleHand(size_t pid) const;
    size_t handSize(size_t pid) const;
    size_t hiddenHandSize(size_t pid) const;

    // Information available to all players about the game
    bool finished() const;
    size_t playerCount() const;
    size_t deckSize() const;
    const std::vector<size_t>& winOrder() const;
    const std::deque<size_t>& attackOrder() const;
    const CardPile& hiddenCards() const;
    const CardSequence& currentAttack() const;
    const CardSequence& currentDefense() const;
    const Card& trumpCard() const;
    Suit trumpSuit() const;
    size_t attackerId() const;
    size_t defenderId() const;
};

inline size_t GameView::playerId() const
{
    return d_pid;
}

}

#endif