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
    std::vector<Card> d_currentAttack;
    std::vector<Card> d_currentDefense;
    std::vector<size_t> d_winOrder;
    std::deque<size_t> d_attackOrder;

public:
    Game(const std::vector<std::shared_ptr<Player>>& players);
    Game(const std::vector<std::shared_ptr<Player>>& players, const GameView& view);
    Game(const Game& game);

    // Public methods used to advance gameplay
    void reset();
    Action nextAction();
    void playAction(const Action& action);
    void play();

    // Private information (protected by the game view)
    const Hand& hand(size_t pid) const;

    // Public information (available to all players)
    bool finished() const;
    size_t playerCount() const;
    size_t deckSize() const;
    const std::vector<size_t>& winOrder() const;
    const std::deque<size_t>& attackOrder() const;
    const std::unordered_set<Card>& discardedCards() const;
    const std::unordered_set<Card>& seenCards() const;
    const std::vector<Card>& currentAttack() const;
    const std::vector<Card>& currentDefense() const;
    const Card& trumpCard() const;
    Suit trumpSuit() const;
    size_t attackerId() const;
    size_t defenderId() const;

private:
    bool validateAttack(const Action& attack) const;
    bool validateDefense(const Action& defense) const;
    void setupViews();
    void finishGoodDefense();
    void finishBadDefense();
    void replenishHand(Hand& hand, size_t max_count);
};

class GameView {
private:
    const Game& d_game;
    size_t d_pid;

public:
    GameView(const Game& game, size_t pid);

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
    const std::unordered_set<Card>& discardedCards() const;
    const std::unordered_set<Card>& seenCards() const;
    const std::vector<Card>& currentAttack() const;
    const std::vector<Card>& currentDefense() const;
    const Card& trumpCard() const;
    Suit trumpSuit() const;
    size_t attackerId() const;
    size_t defenderId() const;
};

inline bool Game::finished() const
{
    return d_winOrder.size() == d_players.size();
}

inline size_t Game::playerCount() const 
{
    return d_players.size();
}

inline size_t Game::deckSize() const
{
    return d_deck.size();
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

inline const std::unordered_set<Card>& Game::discardedCards() const
{
    return d_discards;
}

inline const std::unordered_set<Card>& Game::seenCards() const
{
    return d_seen;
}

inline const std::vector<Card>& Game::currentAttack() const
{
    return d_currentAttack;
}

inline const std::vector<Card>& Game::currentDefense() const
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

inline void Game::setupViews()
{
    for (size_t pid = 0; pid < d_players.size(); pid++) {
        d_views.push_back(GameView(*this, pid));
    }
}

inline void Game::play()
{
    while (!finished()) {
        playAction(nextAction());
    }
}

inline size_t GameView::playerId() const
{
    return d_pid;
}

inline const Hand& GameView::hand() const
{
    return d_game.hand(d_pid);
}

inline size_t GameView::handSize(size_t pid) const
{
    return d_game.hand(pid).size();
}

inline bool GameView::finished() const
{
    return d_game.finished();
}

inline size_t GameView::deckSize() const
{
    return d_game.deckSize();
}

inline size_t GameView::playerCount() const 
{
    return d_game.playerCount();
}

inline const std::vector<size_t>& GameView::winOrder() const
{
    return d_game.winOrder();
}

inline const std::deque<size_t>& GameView::attackOrder() const
{
    return d_game.attackOrder();
}

inline const std::unordered_set<Card>& GameView::discardedCards() const
{
    return d_game.discardedCards();
}

inline const std::unordered_set<Card>& GameView::seenCards() const
{
    return d_game.seenCards();
}

inline const std::vector<Card>& GameView::currentAttack() const
{
    return d_game.currentAttack();
}

inline const std::vector<Card>& GameView::currentDefense() const
{
    return d_game.currentDefense();
}

inline const Card& GameView::trumpCard() const
{
    return d_game.trumpCard();
}

inline Suit GameView::trumpSuit() const
{
    return d_game.trumpSuit();
}

inline size_t GameView::attackerId() const
{
    return d_game.attackerId();
}

inline size_t GameView::defenderId() const
{
    return d_game.defenderId();
}

}

#endif
