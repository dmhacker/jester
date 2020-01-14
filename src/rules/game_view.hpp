#ifndef JESTER_GAME_VIEW_HPP
#define JESTER_GAME_VIEW_HPP

#include <memory>

#include "card.hpp"

namespace jester {

class GameState;

class GameView {
private:
    const GameState& d_state;
    size_t d_pid;

public:
    GameView(const GameState& state, size_t pid);

    // Information relative to the owning player
    size_t playerId() const;
    const Hand& hand() const;
    Hand visibleHand(size_t pid) const;
    size_t handSize(size_t pid) const;
    size_t hiddenHandSize(size_t pid) const;

    // Information available to all players about the game
    bool firstMove() const;
    bool finished() const;
    bool attackerNext() const;
    size_t playerCount() const;
    size_t deckSize() const;
    const std::vector<Action>& nextActions() const;
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
