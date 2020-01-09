#ifndef JESTER_GAME_HPP
#define JESTER_GAME_HPP

#include <memory>
#include <random>

#include "card.hpp"

namespace jester {

class Player;
class GameView;
class Observer;

class GameException : public std::exception {
private:
    std::string d_message;

public:
    GameException(const std::string& message);
    const char* what() const throw();
};

using Players = std::vector<std::shared_ptr<Player>>;
using Observers = std::vector<std::shared_ptr<Observer>>;

class Game {
private:
    Players d_players;
    std::vector<Hand> d_hands;
    Deck d_deck;
    CardPile d_hidden;
    Card d_trump;
    CardSequence d_currentAttack;
    CardSequence d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;
    std::vector<Action> d_nextActions;
    Observers d_observers;

public:
    Game(const Players& players);
    Game(const Players& players, const GameView& view, std::mt19937& rng);
    Game(const Game& game) = default;
    ~Game();

    // Observational methods
    void registerObserver(const std::shared_ptr<Observer>& observer);

    // Public methods used to advance gameplay
    void reset(std::mt19937& rng);
    Action nextAction() const;
    void playAction(const Action& action);
    void play();
    GameView currentPlayerView() const;

    // Private information (protected by the game view)
    const Deck& deck() const;
    const Hand& hand(size_t pid) const;

    // Public information (available to all players)
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
    void validateAction(const Action& action) const;
    void finishGoodDefense();
    void finishBadDefense();
    void replenishHand(Hand& hand, size_t max_count);
    void calculateNextActions();

private:
    friend std::ostream& operator<<(std::ostream& os, const Game& game);
};

inline const char* GameException::what() const throw()
{
    return d_message.c_str();
}

inline void Game::registerObserver(const std::shared_ptr<Observer>& observer)
{
    d_observers.push_back(observer);
}

inline bool Game::finished() const
{
    return d_winOrder.size() == d_players.size();
}

inline bool Game::attackerNext() const
{
    return d_currentAttack.size() == d_currentDefense.size();
}

inline size_t Game::playerCount() const
{
    return d_players.size();
}

inline const std::vector<size_t>& Game::winOrder() const
{
    return d_winOrder;
}

inline const std::deque<size_t>& Game::attackOrder() const
{
    return d_attackOrder;
}

inline const std::vector<Action>& Game::nextActions() const
{
    return d_nextActions;
}

inline const Hand& Game::hand(size_t pid) const
{
    return d_hands[pid];
}

inline const Deck& Game::deck() const
{
    return d_deck;
}

inline const CardPile& Game::hiddenCards() const
{
    return d_hidden;
}

inline const CardSequence& Game::currentAttack() const
{
    return d_currentAttack;
}

inline const CardSequence& Game::currentDefense() const
{
    return d_currentDefense;
}

inline size_t Game::attackerId() const
{
    return d_attackOrder[0];
}

inline size_t Game::defenderId() const
{
    return d_attackOrder[1];
}

inline const Card& Game::trumpCard() const
{
    return d_trump;
}

inline Suit Game::trumpSuit() const
{
    return d_trump.suit();
}

inline size_t Game::currentPlayerId() const
{
    if (attackerNext()) {
        return attackerId();
    } else {
        return defenderId();
    }
}

}

#endif
