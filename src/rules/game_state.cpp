#include "game_state.hpp"
#include "../constants.hpp"
#include "../observers/observer.hpp"
#include "game_view.hpp"

#include <algorithm>

namespace jester {

GameState::GameState(size_t num_players, std::mt19937& rng)
    : d_hands(num_players)
{
    reset(rng);
}

GameState::GameState(const GameView& view, std::mt19937& rng)
    : d_hands(view.playerCount())
    , d_hidden(view.hiddenCards())
    , d_trump(view.trumpCard())
    , d_currentAttack(view.currentAttack())
    , d_currentDefense(view.currentDefense())
    , d_winOrder(view.winOrder())
    , d_attackOrder(view.attackOrder())
    , d_nextActions(view.nextActions())
    , d_firstMove(view.firstMove())
{
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
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        auto const& visible = view.visibleHand(pid);
        d_hands[pid].insert(visible.begin(), visible.end());
    }
    // Deal out hidden cards to player hands first
    size_t hidden_idx = 0;
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        auto& hand = d_hands[pid];
        for (size_t i = 0; i < view.hiddenHandSize(pid); i++) {
            hand.insert(hidden_cards[hidden_idx++]);
        }
    }
    // Put remaining hidden cards in the deck
    if (view.deckSize() > 0) {
        d_deck.insert(d_deck.end(),
            hidden_cards.begin() + hidden_idx,
            hidden_cards.end());
        d_deck.push_back(d_trump);
    }
}

GameState::GameState(const GameState& state)
    : d_hands(state.d_hands)
    , d_deck(state.d_deck)
    , d_hidden(state.d_hidden)
    , d_trump(state.d_trump)
    , d_currentAttack(state.d_currentAttack)
    , d_currentDefense(state.d_currentDefense)
    , d_winOrder(state.d_winOrder)
    , d_attackOrder(state.d_attackOrder)
    , d_nextActions(state.d_nextActions)
    , d_firstMove(state.d_firstMove)
{
}

void GameState::reset(std::mt19937& rng)
{
    for (size_t rank = Constants::instance().MIN_RANK;
         rank <= Constants::instance().MAX_RANK; rank++) {
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
    for (size_t i = 0; i < Constants::instance().HAND_SIZE; i++) {
        for (auto& hand : d_hands) {
            if (d_deck.empty()) {
                break;
            }
            hand.insert(d_deck.front());
            d_deck.pop_front();
        }
    }
    d_attackOrder.clear();
    for (size_t pid = 0; pid < d_hands.size(); pid++) {
        d_attackOrder.push_back(pid);
    }
    d_winOrder.clear();
    d_firstMove = true;
    findNextActions();
}

GameView GameState::currentPlayerView() const
{
    return GameView(*this, currentPlayerId());
}

void GameState::playAction(const Action& action)
{
    if (d_firstMove) {
        if (d_observer != nullptr) {
            d_observer->onGameStart(*this);
        }
        d_firstMove = false;
    }
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (attackerNext()) {
        if (d_observer != nullptr) {
            d_observer->onPostAction(*this, action, true);
        }
        if (action.empty()) {
            finishGoodDefense();
        } else {
            auto& attack_card = action.card();
            d_currentAttack.push_back(attack_card);
            d_hidden.erase(attack_card);
            attack_hand.erase(attack_card);
            // End condition 1: successful attack leaves one remaining player
            if (attack_hand.empty() && d_deck.empty() && d_attackOrder.size() == 2) {
                d_attackOrder.clear();
                d_winOrder.push_back(aid);
                d_nextActions.clear();
                if (d_observer != nullptr) {
                    d_observer->onPlayerWin(*this, aid, d_winOrder.size());
                }
                d_winOrder.push_back(did);
                if (d_observer != nullptr) {
                    d_observer->onPlayerWin(*this, did, d_winOrder.size());
                    d_observer->onGameEnd(*this);
                }
            }
        }
    } else {
        if (d_observer != nullptr) {
            d_observer->onPostAction(*this, action, false);
        }
        if (action.empty()) {
            finishBadDefense();
        } else {
            auto& defense_card = action.card();
            d_currentDefense.push_back(defense_card);
            d_hidden.erase(defense_card);
            defense_hand.erase(defense_card);
            if (defense_hand.empty() || attack_hand.empty()
                || d_currentAttack.size() == Constants::instance().HAND_SIZE) {
                finishGoodDefense();
            }
        }
    }
    findNextActions();
}

void GameState::finishGoodDefense()
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    // All attacking and defending cards are discarded
    d_currentAttack.clear();
    d_currentDefense.clear();
    // Attacker and defender hands are replenished
    replenishHand(attack_hand, Constants::instance().HAND_SIZE);
    replenishHand(defense_hand, Constants::instance().HAND_SIZE);
    // Win and attack orders are updated
    d_attackOrder.pop_front();
    if (attack_hand.empty()) {
        d_winOrder.push_back(aid);
        if (d_observer != nullptr) {
            d_observer->onPlayerWin(*this, aid, d_winOrder.size());
        }
    } else {
        d_attackOrder.push_back(aid);
    }
    if (defense_hand.empty()) {
        d_attackOrder.pop_front();
        d_winOrder.push_back(did);
        if (d_observer != nullptr) {
            d_observer->onPlayerWin(*this, did, d_winOrder.size());
        }
        // End condition 2: successful defense leaves one remaining player
        if (d_attackOrder.size() == 1) {
            auto lid = d_attackOrder.back();
            d_attackOrder.clear();
            d_winOrder.push_back(lid);
            d_nextActions.clear();
            if (d_observer != nullptr) {
                d_observer->onPlayerWin(*this, lid, d_winOrder.size());
                d_observer->onGameEnd(*this);
            }
            return;
        }
    }
    if (d_observer != nullptr) {
        d_observer->onTurnEnd(*this, true);
    }
}

void GameState::finishBadDefense()
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
    replenishHand(attack_hand, Constants::instance().HAND_SIZE);
    replenishHand(defense_hand, Constants::instance().HAND_SIZE);
    // Win and attack orders are updated
    d_attackOrder.pop_front();
    d_attackOrder.pop_front();
    if (attack_hand.empty()) {
        d_winOrder.push_back(aid);
        if (d_observer != nullptr) {
            d_observer->onPlayerWin(*this, aid, d_winOrder.size());
        }
    } else {
        d_attackOrder.push_back(aid);
    }
    d_attackOrder.push_back(did);
    if (d_observer != nullptr) {
        d_observer->onTurnEnd(*this, false);
    }
}

void GameState::replenishHand(Hand& hand, size_t max_count)
{
    while (!d_deck.empty() && hand.size() < max_count) {
        hand.insert(d_deck.front());
        d_deck.pop_front();
    }
}

void GameState::findNextActions()
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
}

void GameState::validateAction(const Action& action) const
{
    if (finished()) {
        throw std::logic_error("The game is finished.");
    }
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (attackerNext()) {
        if (d_currentAttack.size() >= Constants::instance().HAND_SIZE || defense_hand.size() < 1) {
            throw std::logic_error("Too many cards in attack or defense has no cards.");
        }
        if (action.empty()) {
            if (d_currentAttack.empty()) {
                throw std::logic_error("Player must play a card on their first attack.");
            }
        } else {
            auto& attacking = action.card();
            if (attack_hand.find(attacking) == attack_hand.end()) {
                throw std::logic_error("Player must possess the card they are attacking with.");
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
                    throw std::logic_error("Attacking card rank must exist already.");
                }
            }
        }
    } else {
        if (action.empty()) {
            return;
        }
        auto& defending = action.card();
        if (defense_hand.find(defending) == defense_hand.end()) {
            throw std::logic_error("Player must possess the card they are defending with.");
        }
        auto& attacking = d_currentAttack.back();
        bool suit_match = defending.suit() == trumpSuit()
            || defending.suit() == attacking.suit();
        if (!suit_match) {
            throw std::logic_error("Defending card must be the same suit as the attack card");
        }
        bool rank_match = defending.rank() > attacking.rank()
            || (defending.suit() == trumpSuit() && attacking.suit() != trumpSuit());
        if (!rank_match) {
            throw std::logic_error("Defending card must have a higher rank than the attack card");
        }
    }
}

std::ostream& operator<<(std::ostream& os, const GameState& state)
{
    for (size_t pid = 0; pid < state.playerCount(); pid++) {
        os << "  P" << pid << " -- "
           << state.d_hands[pid]
           << std::endl;
    }
    os << "  DK -- "
       << state.d_deck
       << std::endl;
    os << "  CA -- "
       << state.d_currentAttack
       << std::endl;
    os << "  CD -- "
       << state.d_currentDefense
       << std::endl;
    return os;
}

}
