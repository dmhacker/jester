#include "game.hpp"

#include <algorithm>
#include <cassert>

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
    d_discards.clear();
}

void Game::nextTurn()
{
    auto aid = getAttackerId();
    auto did = getDefenderId();
    auto& attacker = d_players[aid];
    auto& defender = d_players[did];
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];

    d_currentAttack.clear();
    d_currentDefense.clear();
    bool firstAttack = true;

    while (d_currentAttack.size() < 6) {
        AttackSequence attack;
        do {
            attack = attacker->attack(d_views[aid], firstAttack);
        } while (!validateAttack(attack, firstAttack));
        d_currentAttack.insert(d_currentAttack.end(), attack.begin(), attack.end());

        // If the attacker played nothing, then the attack is over
        if (attack.empty()) {
            break;
        }
        // Attacker played a valid attack
        else {
            for (auto& card : attack) {
                attack_hand.erase(card);
            }
            if (attack_hand.empty()) {
                break;
            }
        }

        DefenseSequence defense;
        do {
            defense = defender->defend(d_views[did]);
        } while (!validateDefense(defense));
        d_currentDefense.insert(d_currentDefense.end(), defense.begin(), defense.end());

        // If the defender plays nothing, then defense is over
        if (defense.empty()) {
            break;
        }
        // Defender plays a valid defense against the attack
        else {
            for (auto& card : defense) {
                defense_hand.erase(card);
            }
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

bool Game::validateAttack(const AttackSequence& attack, bool firstAttack) const
{
    auto aid = getAttackerId();
    auto did = getDefenderId();
    auto& attack_hand = d_hands[aid];
    auto& defense_hand = d_hands[did];
    if (d_currentAttack.size() + attack.size() > 6
        || attack.size() > defense_hand.size()) {
        return false;
    }
    for (auto& card : attack) {
        if (attack_hand.find(card) == attack_hand.end()) {
            return false;
        }
    }
    if (firstAttack) {
        return attack_hand.size() >= 1;
    } else {
        if (attack_hand.empty()) {
            return true;
        }
        std::unordered_set<size_t> valid_ranks;
        for (auto& card : d_currentAttack) {
            valid_ranks.insert(card.rank());
        }
        for (auto& card : d_currentDefense) {
            valid_ranks.insert(card.rank());
        }
        for (auto& card : attack) {
            if (valid_ranks.find(card.rank()) == valid_ranks.end()) {
                return false;
            }
        }
        return true;
    }
}

bool Game::validateDefense(const DefenseSequence& defense) const
{
    if (defense.empty()) {
        return true;
    }
    auto did = getDefenderId();
    auto& defense_hand = d_hands[did];
    for (auto& card : defense) {
        if (defense_hand.find(card) == defense_hand.end()) {
            return false;
        }
    }
    for (size_t i = 0; i < defense.size(); i++) {
        auto& defending = defense[i];
        auto& attacking = d_currentAttack[i + d_currentDefense.size()];
        if (defending.suit() == getTrumpSuit()) {
            if (attacking.suit() == getTrumpSuit()
                && attacking.rank() >= defending.rank()) {
                return false;
            }
        } else {
            if (attacking.suit() != defending.suit()
                || attacking.rank() >= defending.rank()) {
                return false;
            }
        }
    }
    return true;
}

GameView::GameView(const Game& game, size_t pid)
    : d_game(game)
    , d_pid(pid)
{
}

}
