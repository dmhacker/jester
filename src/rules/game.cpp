#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace jester {

Game::Game(const std::vector<std::shared_ptr<Player>>& players)
    : d_players(players)
    , d_hands(players.size())
{
    assert(players.size() >= 2);
    setupViews();
    reset();
}

Game::Game(const std::vector<std::shared_ptr<Player>>& players, const GameView& view)
    : d_players(players)
    , d_hands(players.size())
    , d_hidden(view.hiddenCards())
    , d_trump(view.trumpCard())
    , d_currentAttack(view.currentAttack())
    , d_currentDefense(view.currentDefense())
    , d_winOrder(view.winOrder())
    , d_attackOrder(view.attackOrder())
{
    assert(players.size() == view.playerCount());
    setupViews();
    // Cards in hidden set may be in player hands or may be in the deck
    // Convert set into vector and then shuffle the vector
    std::vector<Card> hidden_cards;
    hidden_cards.insert(hidden_cards.end(), d_hidden.begin(), d_hidden.end());
    std::random_shuffle(hidden_cards.begin(), hidden_cards.end());
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

Game::Game(const Game& game)
    : d_players(game.d_players)
    , d_hands(game.d_hands)
    , d_deck(game.d_deck)
    , d_hidden(game.d_hidden)
    , d_trump(game.d_trump)
    , d_currentAttack(game.d_currentAttack)
    , d_currentDefense(game.d_currentDefense)
    , d_winOrder(game.d_winOrder)
    , d_attackOrder(game.d_attackOrder)
{
    setupViews();
}

void Game::reset()
{
    for (size_t rank = 6; rank <= 14; rank++) {
        d_deck.push_back(Card(rank, Suit::hearts));
        d_deck.push_back(Card(rank, Suit::diamonds));
        d_deck.push_back(Card(rank, Suit::spades));
        d_deck.push_back(Card(rank, Suit::clubs));
    }
    std::random_shuffle(d_deck.begin(), d_deck.end());
    d_trump = d_deck.back();
    d_hidden.clear();
    d_hidden.insert(d_deck.begin(), d_deck.end());
    d_hidden.erase(d_trump);
    for (auto& hand : d_hands) {
        for (size_t i = 0; i < 6; i++) {
            hand.insert(d_deck.front());
            d_deck.pop_front();
        }
    }
    d_attackOrder.clear();
    for (size_t pid = 0; pid < d_players.size(); pid++) {
        d_attackOrder.push_back(pid);
    }
    d_winOrder.clear();
    std::cerr << "Trump suit is " << to_string(trumpSuit()) << "." << std::endl;
}

Action Game::nextAction()
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attacker = d_players[aid];
    auto& defender = d_players[did];
    if (d_currentAttack.size() == d_currentDefense.size()) {
        Action attack;
        do {
            attack = attacker->attack(d_views[aid], std::chrono::milliseconds(3000));
        } while (!validateAttack(attack));
        if (attack.empty()) {
            std::cerr << "Attacker " << aid << " passes on his turn." << std::endl;
        } else {
            std::cerr << "Attacker " << aid << " plays " << attack.card() << "." << std::endl;
        }
        return attack;
    } else {
        Action defense;
        do {
            defense = defender->defend(d_views[did], std::chrono::milliseconds(3000));
        } while (!validateDefense(defense));
        if (defense.empty()) {
            std::cerr << "Defender " << did << " gives up on his defense." << std::endl;
        } else {
            std::cerr << "Defender " << did << " plays " << defense.card() << "." << std::endl;
        }
        return defense;
    }
}

void Game::playAction(const Action& action)
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];

    if (d_currentAttack.size() == d_currentDefense.size()) {
        if (action.empty()) {
            assert(d_currentAttack.size() > 0);
            finishGoodDefense();
        } else {
            auto& attack_card = action.card();
            d_currentAttack.push_back(attack_card);
            d_hidden.erase(attack_card);
            attack_hand.erase(attack_card);
            // End condition 1: defender is the losing player
            if (attack_hand.empty() && d_deck.empty() && d_winOrder.size() + 2 == d_players.size()) {
                d_winOrder.push_back(aid);
                d_winOrder.push_back(did);
                d_attackOrder.clear();
                std::cerr << "Player " << aid << " has finished the game!" << std::endl;
                std::cerr << "Player " << did << " has finished the game!" << std::endl;
            }
        }
    } else {
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
        std::cerr << "Player " << aid << " has finished the game!" << std::endl;
    } else {
        d_attackOrder.push_back(aid);
    }
    if (defense_hand.empty()) {
        d_attackOrder.pop_front();
        d_winOrder.push_back(did);
        std::cerr << "Player " << did << " has finished the game!" << std::endl;
        // End condition 2: attacker is the losing player
        if (d_winOrder.size() + 1 == d_players.size()) {
            d_winOrder.push_back(aid);
            d_attackOrder.clear();
            std::cerr << "Player " << aid << " has finished the game!" << std::endl;
        }
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
        std::cerr << "Player " << aid << " has finished the game!" << std::endl;
    } else {
        d_attackOrder.push_back(aid);
    }
    d_attackOrder.push_back(did);
}

bool Game::validateAttack(const Action& attack) const
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (d_currentAttack.size() >= 6 || defense_hand.size() < 1) {
        return false;
    }
    if (attack.empty()) {
        return !d_currentAttack.empty();
    } else {
        auto& attacking = attack.card();
        if (attack_hand.find(attacking) == attack_hand.end()) {
            return false;
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
                return false;
            }
        }
        return true;
    }
}

bool Game::validateDefense(const Action& defense) const
{
    if (defense.empty()) {
        return true;
    }
    auto did = defenderId();
    auto& defense_hand = d_hands[did];
    auto& defending = defense.card();
    if (defense_hand.find(defending) == defense_hand.end()) {
        return false;
    }
    auto& attacking = d_currentAttack.back();
    if (defending.suit() == trumpSuit()) {
        if (attacking.suit() == trumpSuit()
            && attacking.rank() >= defending.rank()) {
            return false;
        }
    } else {
        if (attacking.suit() != defending.suit()
            || attacking.rank() >= defending.rank()) {
            return false;
        }
    }
    return true;
}

void Game::replenishHand(Hand& hand, size_t max_count)
{
    while (!d_deck.empty() && hand.size() < max_count) {
        hand.insert(d_deck.front());
        d_deck.pop_front();
    }
}

GameView::GameView(const Game& game, size_t pid)
    : d_game(game)
    , d_pid(pid)
{
}

Hand GameView::visibleHand(size_t pid) const
{
    Hand hand;
    auto& hidden = d_game.hiddenCards();
    for (auto& card : d_game.hand(pid)) {
        if (d_pid == pid || hidden.find(card) == hidden.end()) {
            hand.insert(card);
        }
    }
    return hand;
}

size_t GameView::hiddenHandSize(size_t pid) const
{
    if (d_pid == pid) {
        return 0;
    } else {
        size_t cnt = 0;
        auto& hidden = d_game.hiddenCards();
        for (auto& card : d_game.hand(pid)) {
            if (hidden.find(card) != hidden.end()) {
                cnt++;
            }
        }
        return cnt;
    }
}

}
