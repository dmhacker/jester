#ifndef JESTER_GAME_STATE_HPP
#define JESTER_GAME_STATE_HPP

#include <memory>
#include <random>
#include <stda/erased_ptr.hpp>

#include "card.hpp"

namespace jester {

class GameView;
class Observer;

class GameState {
private:
    std::vector<Hand> d_hands;
    Deck d_deck;
    CardPile d_hidden;
    Card d_trump;
    CardSequence d_currentAttack;
    CardSequence d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;
    std::vector<Action> d_nextActions;
    stda::erased_ptr<Observer> d_observer;
    size_t d_turn;

public:
    GameState() = default;
    GameState(size_t num_players, std::mt19937& rng);
    GameState(const GameView& view, std::mt19937& rng);
    GameState(const GameState& state);

    // Observational methods
    Observer* observer() const;
    void setObserver(stda::erased_ptr<Observer>&& observer);

    // Public methods used to advance gameplay
    void reset(std::mt19937& rng);
    void playAction(const Action& action);
    GameView currentPlayerView() const;
    void validateAction(const Action& action) const;

    // Private information (protected by the game view)
    const Deck& deck() const;
    const Hand& hand(size_t pid) const;

    // Public information (available to all players)
    size_t turn() const;
    bool finished() const;
    bool attackerNext() const;
    size_t playerCount() const;
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
    size_t currentPlayerId() const;

private:
    void finishGoodDefense();
    void finishBadDefense();
    void replenishHand(Hand& hand, size_t max_count);
    void findNextActions();

private:
    friend std::ostream& operator<<(std::ostream& os, const GameState& state);
};

inline Observer* GameState::observer() const
{
    return d_observer.get();
}

inline void GameState::setObserver(stda::erased_ptr<Observer>&& observer)
{
    d_observer = std::move(observer);
}

inline size_t GameState::turn() const
{
    return d_turn;
}

inline bool GameState::finished() const
{
    return d_winOrder.size() == d_hands.size();
}

inline bool GameState::attackerNext() const
{
    return d_currentAttack.size() == d_currentDefense.size();
}

inline size_t GameState::playerCount() const
{
    return d_hands.size();
}

inline const std::vector<size_t>& GameState::winOrder() const
{
    return d_winOrder;
}

inline const std::deque<size_t>& GameState::attackOrder() const
{
    return d_attackOrder;
}

inline const std::vector<Action>& GameState::nextActions() const
{
    return d_nextActions;
}

inline const Hand& GameState::hand(size_t pid) const
{
    return d_hands[pid];
}

inline const Deck& GameState::deck() const
{
    return d_deck;
}

inline const CardPile& GameState::hiddenCards() const
{
    return d_hidden;
}

inline const CardSequence& GameState::currentAttack() const
{
    return d_currentAttack;
}

inline const CardSequence& GameState::currentDefense() const
{
    return d_currentDefense;
}

inline size_t GameState::attackerId() const
{
    return d_attackOrder[0];
}

inline size_t GameState::defenderId() const
{
    return d_attackOrder[1];
}

inline const Card& GameState::trumpCard() const
{
    return d_trump;
}

inline Suit GameState::trumpSuit() const
{
    return d_trump.suit();
}

inline size_t GameState::currentPlayerId() const
{
    if (attackerNext()) {
        return attackerId();
    } else {
        return defenderId();
    }
}

}

#endif
