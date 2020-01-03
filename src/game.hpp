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
    std::unordered_set<Card> d_discards;
    std::unordered_set<Card> d_seen;
    Card d_trump;
    AttackSequence d_currentAttack;
    DefenseSequence d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;

public:
    Game(const std::vector<std::shared_ptr<Player>>& players);

    // Public methods used to advance gameplay 
    void reset();
    void nextTurn();
    void play();

    // Private information (protected by the game view)
    const Hand& getHand(size_t pid) const;

    // Public information (available to all players)
    bool isFinished() const;
    const std::vector<size_t>& getWinOrder() const;
    const std::unordered_set<Card>& getDiscardedCards() const;
    const std::unordered_set<Card>& getSeenCards() const;
    Suit getTrumpSuit() const;
    size_t getAttackerId() const;
    size_t getDefenderId() const;

    // Methods used to change game state (dangerous)
    void setAttackOrder(const std::deque<size_t>& order);
    void setHands(const std::vector<Hand>& hands);
    void setDeck(const Deck& deck);
    void setTrumpCard(const Card& card);

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

    // Information relative to the owning player
    size_t getPlayerId() const;
    const Hand& getHand() const;
    const Hand& getVisibleHand(size_t pid) const;
    size_t getHandSize(size_t pid) const;
    size_t getHiddenHandSize(size_t pid) const;

    // Information available to all players about the game
    bool isFinished() const;
    const std::vector<size_t>& getWinOrder() const;
    const std::unordered_set<Card>& getDiscardedCards() const;
    const std::unordered_set<Card>& getSeenCards() const;
    Suit getTrumpSuit() const;
    size_t getAttackerId() const;
    size_t getDefenderId() const;
};

inline bool Game::isFinished() const
{
    return d_winOrder.size() == d_players.size();
}

inline const std::vector<size_t>& Game::getWinOrder() const
{
    return d_winOrder;
}

inline const Hand& Game::getHand(size_t pid) const
{
    return d_hands[pid];
}

inline const std::unordered_set<Card>& Game::getDiscardedCards() const
{
    return d_discards;
}

inline const std::unordered_set<Card>& Game::getSeenCards() const
{
    return d_seen;
}

inline size_t Game::getAttackerId() const
{
    return d_attackOrder[0];
}

inline size_t Game::getDefenderId() const
{
    return d_attackOrder[1];
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

}

#endif
