#ifndef JESTER_GAME_HPP
#define JESTER_GAME_HPP

#include <memory>

#include "card.hpp"
#include "player.hpp"

namespace jester {

class GameView;

class Game {
private:
    std::vector<std::shared_ptr<Player>> d_players;
    std::vector<GameView> d_views;
    std::vector<Hand> d_hands;
    Deck d_deck;
    DiscardPile d_discards;
    Card d_trump;
    AttackSequence d_currentAttack;
    DefenseSequence d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;

public:
    Game(const std::vector<std::shared_ptr<Player>>& players);
    void reset();
    void nextTurn();
    void play();
    bool isFinished() const;
    const std::vector<size_t>& getWinOrder() const;
    const DiscardPile& getDiscards() const;
    const Card& getTrumpCard() const;
    Suit getTrumpSuit() const;
    size_t getAttackerId() const;
    size_t getDefenderId() const;
    size_t getTurns() const;
    void setAttackOrder(const std::deque<size_t>& order);
    void setHands(const std::vector<Hand>& hands);
    void setDeck(const Deck& deck);
    void setTrumpCard(const Card& card);
    void setTurns(size_t turns);

private:
    bool validateAttack(const AttackSequence& attack, bool firstAttack) const;
    bool validateDefense(const DefenseSequence& defense) const;
};

class GameView {
private:
    const Game& d_game;
    size_t d_pid;

public:
    GameView(const Game& game, size_t pid);
};

inline bool Game::isFinished() const
{
    return d_winOrder.size() == d_players.size();
}

inline const std::vector<size_t>& Game::getWinOrder() const
{
    return d_winOrder;
}

inline size_t Game::getAttackerId() const
{
    return d_attackOrder[0];
}

inline size_t Game::getDefenderId() const
{
    return d_attackOrder[1];
}

inline const DiscardPile& Game::getDiscards() const
{
    return d_discards;
}

inline const Card& Game::getTrumpCard() const
{
    return d_trump;
}

inline Suit Game::getTrumpSuit() const
{
    return d_trump.suit();
}

inline void Game::setAttackOrder(const std::deque<size_t>& order)
{
    d_attackOrder = order;
}

inline void Game::setHands(const std::vector<Hand>& hands)
{
    d_hands = hands;
}

inline void Game::setDeck(const Deck& deck)
{
    d_deck = deck;
}

inline void Game::setTrumpCard(const Card& card)
{
    d_trump = card;
}

inline void Game::play()
{
    while (!isFinished()) {
        nextTurn();
    }
}

}

#endif
