#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace jester {

Game::Game(const std::vector<std::shared_ptr<Player>>& players)
    : d_players(players)
    , d_hands(players.size())
{
    assert(players.size() >= 2 && players.size() <= 6);
    for (size_t pid = 0; pid < players.size(); pid++) {
        d_views.push_back(GameView(*this, pid));
    }
    reset();
}

Game::Game(const Game& game)
    : d_players(game.d_players)
    , d_hands(game.d_hands)
    , d_deck(game.d_deck)
    , d_discards(game.d_discards)
    , d_seen(game.d_seen)
    , d_trump(game.d_trump)
    , d_currentAttack(game.d_currentAttack)
    , d_currentDefense(game.d_currentDefense)
    , d_winOrder(game.d_winOrder)
    , d_attackOrder(game.d_attackOrder)
{
    for (size_t pid = 0; pid < d_players.size(); pid++) {
        d_views.push_back(GameView(*this, pid));
    }
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
    d_seen.clear();
    d_seen.insert(d_trump);
    d_discards.clear();
    std::cerr << "Trump suit is " << to_string(trumpSuit()) << "." << std::endl;
}

void Game::nextTurn()
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attacker = d_players[aid];
    auto& defender = d_players[did];
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];

    d_currentAttack.clear();
    d_currentDefense.clear();
    bool firstAttack = true;

    while (d_currentAttack.size() < 6) {
        std::shared_ptr<Card> attack;
        do {
            attack = attacker->attack(d_views[aid], firstAttack);
        } while (!validateAttack(attack, firstAttack));

        // If the attacker played nothing, then the attack is over
        if (attack == nullptr) {
            std::cerr << "Attacker " << aid << " did not play anything." << std::endl;
            break;
        }
        // Attacker played a valid attack
        else {
            Card attack_card = *attack;
            std::cerr << "Attacker " << aid << " played " << attack_card << "." << std::endl;
            d_currentAttack.push_back(attack_card);
            attack_hand.erase(attack_card);
            d_seen.insert(attack_card);
            if (attack_hand.empty()) {
                break;
            }
        }

        std::shared_ptr<Card> defense;
        do {
            defense = defender->defend(d_views[did]);
        } while (!validateDefense(defense));

        // If the defender plays nothing, then defense is over
        if (defense == nullptr) {
            std::cerr << "Defender " << did << " did not play anything." << std::endl;
            break;
        }
        // Defender plays a valid defense against the attack
        else {
            Card defense_card = *defense;
            std::cerr << "Defender " << did << " played " << defense_card << "." << std::endl;
            d_currentDefense.push_back(defense_card);
            defense_hand.erase(defense_card);
            d_seen.insert(defense_card);
            if (defense_hand.empty()) {
                break;
            }
        }

        firstAttack = false;
    }

    // Attacker can win regardless of the defense played
    bool attackerWon = false;
    if (attack_hand.empty() && d_deck.empty()) {
        d_winOrder.push_back(aid);
        attackerWon = true;
        if (d_winOrder.size() + 1 == d_players.size()) {
            d_winOrder.push_back(did);
            return;
        }
    }

    if (d_currentAttack.size() == d_currentDefense.size()) {
        // Defense was successful, cards are discarded
        for (auto& card : d_currentAttack) {
            d_discards.insert(card);
        }
        for (auto& card : d_currentDefense) {
            d_discards.insert(card);
        }
        // Defender can only win if they play a successful defense
        bool defenderWon = false;
        if (defense_hand.empty() && d_deck.empty()) {
            d_winOrder.push_back(did);
            defenderWon = true;
            if (d_winOrder.size() + 1 == d_players.size()) {
                d_winOrder.push_back(aid);
                return;
            }
        }
        // Attacker sent to the back of the line, defender is the new attacker
        d_attackOrder.pop_front();
        if (!attackerWon) {
            d_attackOrder.push_back(aid);
        }
        if (defenderWon) {
            d_attackOrder.pop_front();
        }
    } else {
        // Defense was unsuccessful, all cards go the defender
        for (auto& card : d_currentAttack) {
            defense_hand.insert(card);
        }
        for (auto& card : d_currentDefense) {
            defense_hand.insert(card);
        }
        // Attacker and defender are both sent to the back
        d_attackOrder.pop_front();
        d_attackOrder.pop_front();
        if (!attackerWon) {
            d_attackOrder.push_back(aid);
        }
        d_attackOrder.push_back(did);
    }

    // Replenish attacker and defender hands if we can
    while (!d_deck.empty() && attack_hand.size() < 6) {
        attack_hand.insert(d_deck.front());
        d_deck.pop_front();
    }
    while (!d_deck.empty() && defense_hand.size() < 6) {
        defense_hand.insert(d_deck.front());
        d_deck.pop_front();
    }
}

bool Game::validateAttack(const std::shared_ptr<Card>& attack, bool firstAttack) const
{
    auto aid = attackerId();
    auto did = defenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (d_currentAttack.size() >= 6 || defense_hand.size() < 1) {
        return false;
    }
    if (attack == nullptr) {
        return !firstAttack;
    } else {
        auto attacking = *attack;
        if (attack_hand.find(attacking) == attack_hand.end()) {
            return false;
        }
        if (!firstAttack) {
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

bool Game::validateDefense(const std::shared_ptr<Card>& defense) const
{
    if (defense == nullptr) {
        return true;
    }
    auto did = defenderId();
    auto& defense_hand = d_hands[did];
    auto defending = *defense;
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

void Game::play()
{
    while (!finished()) {
        nextTurn();
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
    auto& seen = d_game.seenCards();
    for (auto& card : d_game.hand(pid)) {
        if (d_pid == pid || seen.find(card) != seen.end()) {
            hand.insert(card);
        }
    }
    return hand;
}

size_t GameView::hiddenHandSize(size_t pid) const
{
    if (d_pid == pid) {
        return handSize(pid);
    } else {
        size_t cnt = 0;
        auto& seen = d_game.seenCards();
        for (auto& card : d_game.hand(pid)) {
            if (seen.find(card) == seen.end()) {
                cnt++;
            }
        }
        return cnt;
    }
}

}
