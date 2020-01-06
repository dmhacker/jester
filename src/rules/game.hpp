#ifndef JESTER_GAME_HPP
#define JESTER_GAME_HPP

#include <memory>

#include "card.hpp"

namespace jester {

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container) {
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& container) {
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& container) {
    os << "{";
    size_t i = 0;
    for (auto& item : container) {
        os << item;
        if (i < container.size() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "}";
    return os;
}

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

class Game {
private:
    std::vector<std::shared_ptr<Player>> d_players;
    std::vector<GameView> d_views;
    std::vector<Hand> d_hands;
    Deck d_deck;
    CardPile d_hidden;
    Card d_trump;
    CardSequence d_currentAttack;
    CardSequence d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;
    std::vector<std::shared_ptr<Observer>> d_observers;

public:
    Game(const std::vector<std::shared_ptr<Player>>& players);
    Game(const std::vector<std::shared_ptr<Player>>& players, const GameView& view);
    Game(const Game& game);
    ~Game();

    // Observational methods
    void registerObserver(const std::shared_ptr<Observer>& observer);

    // Public methods used to advance gameplay
    void reset();
    Action nextAction() const;
    void playAction(const Action& action);
    void play();

    // Private information (protected by the game view)
    const Deck& deck() const;
    const Hand& hand(size_t pid) const;

    // Public information (available to all players)
    bool finished() const;
    size_t playerCount() const;
    std::vector<Action> nextActions() const;
    const std::vector<size_t>& winOrder() const;
    const std::deque<size_t>& attackOrder() const;
    const CardPile& hiddenCards() const;
    const CardSequence& currentAttack() const;
    const CardSequence& currentDefense() const;
    const Card& trumpCard() const;
    Suit trumpSuit() const;
    size_t attackerId() const;
    size_t defenderId() const;

private:
    void validateAttack(const Action& attack) const;
    void validateDefense(const Action& defense) const;
    void setupViews();
    void finishGoodDefense();
    void finishBadDefense();
    void replenishHand(Hand& hand, size_t max_count);

private:
    friend std::ostream& operator<<(std::ostream& os, const Game& game);
};

inline const char* GameException::what() const throw() {
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

inline void Game::play()
{
    while (!finished()) {
        playAction(nextAction());
    }
}

}

#endif
