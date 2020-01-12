#include "game.hpp"
#include "../observers/observer.hpp"
#include "../players/player.hpp"
#include "game_view.hpp"

#include <algorithm>
#include <cassert>

namespace jester {

GameException::GameException(const std::string& message)
    : d_message(message)
{
}

Game::Game(const Players& players)
    : d_players(players)
    , d_hands(players.size())
{
    assert(players.size() >= 2);
    std::mt19937 rng(std::random_device {}());
    reset(rng);
}

Game::Game(const Players& players, const GameView& view, std::mt19937& rng)
    : d_players(players)
    , d_hands(players.size())
    , d_hidden(view.hiddenCards())
    , d_trump(view.trumpCard())
    , d_currentAttack(view.currentAttack())
    , d_currentDefense(view.currentDefense())
    , d_winOrder(view.winOrder())
    , d_attackOrder(view.attackOrder())
    , d_nextActions(view.nextActions())
{
    assert(players.size() == view.playerCount());
    // Cards in hidden set may be in player hands or may be in the deck
    // Convert set into vector and then shuffle the vector
    std::vector<Card> hidden_cards;
    for (auto& card : d_hidden) {
        if (view.hand().find(card) == view.hand().end()) {
            hidden_cards.push_back(card);
        }
    }
    std::shuffle(hidden_cards.begin(), hidden_cards.end(), rng);
    // Populate player hands with their known cards
    for (size_t pid = 0; pid < players.size(); pid++) {
        auto const& visible = view.visibleHand(pid);
        d_hands[pid].insert(visible.begin(), visible.end());
    }
    // Deal out hidden cards to player hands first
    size_t hidden_idx = 0;
    for (size_t pid = 0; pid < players.size(); pid++) {
        auto& hand = d_hands[pid];
        for (size_t i = 0; i < view.hiddenHandSize(pid); i++) {
            hand.insert(hidden_cards[hidden_idx++]);
        }
    }
    // Put remaining hidden cards in the deck
    if (view.deckSize() > 0) {
        d_deck.insert(d_deck.end(), hidden_cards.begin() + hidden_idx, hidden_cards.end());
        d_deck.push_back(d_trump);
    }
    assert(d_deck.size() == view.deckSize());
}

void Game::reset(std::mt19937& rng)
{
    for (size_t rank = 6; rank <= 7; rank++) {
        d_deck.push_back(Card(rank, Suit::hearts));
        d_deck.push_back(Card(rank, Suit::diamonds));
        d_deck.push_back(Card(rank, Suit::spades));
        d_deck.push_back(Card(rank, Suit::clubs));
    }
    std::shuffle(d_deck.begin(), d_deck.end(), rng);
    d_trump = d_deck.back();
    d_hidden.clear();
    d_hidden.insert(d_deck.begin(), d_deck.end());
    d_hidden.erase(d_trump);
    for (size_t i = 0; i < 6; i++) {
        for (auto& hand : d_hands) {
            if (d_deck.empty()) {
                break;
            }
            hand.insert(d_deck.front());
            d_deck.pop_front();
        }
    }
    d_attackOrder.clear();
    for (size_t pid = 0; pid < d_players.size(); pid++) {
        d_attackOrder.push_back(pid);
    }
    d_winOrder.clear();
    calculateNextActions();
}

GameView Game::currentPlayerView() const
{
    return GameView(*this, currentPlayerId());
}

Action Game::nextAction() const
{
    auto& player = d_players[currentPlayerId()];
    Action action = player->nextAction(currentPlayerView());
    return action;
}

void Game::calculateNextActions()
{
    d_nextActions.clear();
    if (finished()) {
        return;
    }
    auto aid = attackerId();
    auto did = defenderId();
    if (attackerNext()) {
        if (d_currentAttack.empty()) {
            for (auto& card : d_hands[aid]) {
                d_nextActions.push_back(Action(card));
            }
        } else {
            d_nextActions.push_back(Action());
            std::unordered_set<size_t> valid_ranks;
            for (auto& card : d_currentAttack) {
                valid_ranks.insert(card.rank());
            }
            for (auto& card : d_currentDefense) {
                valid_ranks.insert(card.rank());
            }
            for (auto& card : d_hands[aid]) {
                if (valid_ranks.find(card.rank()) != valid_ranks.end()) {
                    d_nextActions.push_back(Action(card));
                }
            }
        }
    } else {
        d_nextActions.push_back(Action());
        auto attacking = d_currentAttack.back();
        for (auto& card : d_hands[did]) {
            if (attacking.suit() == trumpSuit()) {
                if (card.suit() == trumpSuit() && card.rank() > attacking.rank()) {
                    d_nextActions.push_back(Action(card));
                }
            } else {
                if (card.suit() == trumpSuit()
                    || (card.suit() == attacking.suit()
                        && card.rank() > attacking.rank())) {
                    d_nextActions.push_back(Action(card));
                }
            }
        }
    }
    std::sort(d_nextActions.begin(), d_nextActions.end());
}

void Game::play()
{
    for (auto& observer : d_observers) {
        observer->onGameStart(*this);
    }
    while (!finished()) {
        playAction(nextAction());
    }
    for (auto& observer : d_observers) {
        observer->onGameEnd(*this);
    }
}

void Game::playAction(const Action& action)
{
    validateAction(action);
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (attackerNext()) {
        for (auto& observer : d_observers) {
            observer->onPostAction(*this, action, true);
        }
        if (action.empty()) {
            finishGoodDefense();
        } else {
            auto& attack_card = action.card();
            d_currentAttack.push_back(attack_card);
            d_hidden.erase(attack_card);
            attack_hand.erase(attack_card);
            // End condition 1: defender is the losing player
            if (attack_hand.empty() && d_deck.empty() && d_winOrder.size() + 2 == d_players.size()) {
                d_attackOrder.clear();
                d_winOrder.push_back(aid);
                d_nextActions.clear();
                for (auto& observer : d_observers) {
                    observer->onPlayerWin(*this, aid, d_winOrder.size());
                }
                d_winOrder.push_back(did);
                for (auto& observer : d_observers) {
                    observer->onPlayerWin(*this, did, d_winOrder.size());
                }
            }
        }
    } else {
        for (auto& observer : d_observers) {
            observer->onPostAction(*this, action, false);
        }
        if (action.empty()) {
            finishBadDefense();
        } else {
            auto& defense_card = action.card();
            d_currentDefense.push_back(defense_card);
            d_hidden.erase(defense_card);
            defense_hand.erase(defense_card);
            if (defense_hand.empty() || attack_hand.empty() || d_currentAttack.size() == 6) {
                finishGoodDefense();
            }
        }
    }
    calculateNextActions();
}

void Game::finishGoodDefense()
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    // All attacking and defending cards are discarded
    d_currentAttack.clear();
    d_currentDefense.clear();
    // Attacker and defender hands are replenished
    replenishHand(attack_hand, 6);
    replenishHand(defense_hand, 6);
    // Win and attack orders are updated
    d_attackOrder.pop_front();
    if (attack_hand.empty()) {
        d_winOrder.push_back(aid);
        for (auto& observer : d_observers) {
            observer->onPlayerWin(*this, aid, d_winOrder.size());
        }
    } else {
        d_attackOrder.push_back(aid);
    }
    if (defense_hand.empty()) {
        d_attackOrder.pop_front();
        d_winOrder.push_back(did);
        for (auto& observer : d_observers) {
            observer->onPlayerWin(*this, did, d_winOrder.size());
        }
        // End condition 2: attacker is the losing player
        if (d_winOrder.size() + 1 == d_players.size()) {
            d_attackOrder.clear();
            d_winOrder.push_back(aid);
            d_nextActions.clear();
            for (auto& observer : d_observers) {
                observer->onPlayerWin(*this, aid, d_winOrder.size());
            }
            return;
        }
    }
    for (auto& observer : d_observers) {
        observer->onTurnEnd(*this, true);
    }
}

void Game::finishBadDefense()
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    // All attacking and defending cards go the defender
    defense_hand.insert(d_currentAttack.begin(), d_currentAttack.end());
    defense_hand.insert(d_currentDefense.begin(), d_currentDefense.end());
    d_currentAttack.clear();
    d_currentDefense.clear();
    // Attacker and defender hands are replenished
    replenishHand(attack_hand, 6);
    replenishHand(defense_hand, 6);
    // Win and attack orders are updated
    d_attackOrder.pop_front();
    d_attackOrder.pop_front();
    if (attack_hand.empty()) {
        d_winOrder.push_back(aid);
        for (auto& observer : d_observers) {
            observer->onPlayerWin(*this, aid, d_winOrder.size());
        }
    } else {
        d_attackOrder.push_back(aid);
    }
    d_attackOrder.push_back(did);
    for (auto& observer : d_observers) {
        observer->onTurnEnd(*this, false);
    }
}

void Game::validateAction(const Action& action) const
{
    if (finished()) {
        throw GameException("The game is finished.");
    }
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (attackerNext()) {
        if (d_currentAttack.size() >= 6 || defense_hand.size() < 1) {
            throw GameException("Too many cards in attack or defense has no cards.");
        }
        if (action.empty()) {
            if (d_currentAttack.empty()) {
                throw GameException("Player must play a card on their first attack.");
            }
        } else {
            auto& attacking = action.card();
            if (attack_hand.find(attacking) == attack_hand.end()) {
                throw GameException("Player must possess the card they are attacking with.");
            }
            if (!d_currentAttack.empty()) {
                std::unordered_set<size_t> valid_ranks;
                for (auto& card : d_currentAttack) {
                    valid_ranks.insert(card.rank());
                }
                for (auto& card : d_currentDefense) {
                    valid_ranks.insert(card.rank());
                }
                if (valid_ranks.find(attacking.rank()) == valid_ranks.end()) {
                    throw GameException("Attacking card rank must exist already.");
                }
            }
        }
    } else {
        if (action.empty()) {
            return;
        }
        auto& defending = action.card();
        if (defense_hand.find(defending) == defense_hand.end()) {
            throw GameException("Player must possess the card they are defending with.");
        }
        auto& attacking = d_currentAttack.back();
        bool suit_match = defending.suit() == trumpSuit()
            || defending.suit() == attacking.suit();
        if (!suit_match) {
            throw GameException("Defending card must be the same suit as the attack card");
        }
        bool rank_match = defending.rank() > attacking.rank() || (defending.suit() == trumpSuit() && attacking.suit() != trumpSuit());
        if (!rank_match) {
            throw GameException("Defending card must have a higher rank than the attack card");
        }
    }
}

void Game::replenishHand(Hand& hand, size_t max_count)
{
    while (!d_deck.empty() && hand.size() < max_count) {
        hand.insert(d_deck.front());
        d_deck.pop_front();
    }
}

std::ostream& operator<<(std::ostream& os, const Game& game)
{
    for (size_t pid = 0; pid < game.playerCount(); pid++) {
        os << "  P" << pid << " -- "
           << game.d_hands[pid]
           << std::endl;
    }
    os << "  DK -- "
       << game.d_deck
       << std::endl;
    os << "  CA -- "
       << game.d_currentAttack
       << std::endl;
    os << "  CD -- "
       << game.d_currentDefense
       << std::endl;
    return os;
}

}
