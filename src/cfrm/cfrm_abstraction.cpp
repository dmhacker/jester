#include "cfrm_abstraction.hpp"
#include "../rules/game_view.hpp"

namespace jester {

constexpr static uint8_t MAX_CARDS = 36;
constexpr static uint8_t CARD_HIDDEN = 6;
constexpr static uint8_t CARD_DISCARDED = 7;
constexpr static uint8_t CARD_DECK_BOTTOM = 8;
constexpr static uint8_t CARD_IN_ATTACK = 9;
constexpr static uint8_t CARD_IN_DEFENSE = 10;
constexpr static uint8_t CARD_LAST_IN_ATTACK = 11;

CFRMAbstraction::CFRMAbstraction(const GameView& view)
    : d_cardStates(MAX_CARDS)
    , d_hiddenHands(view.playerCount())
    , d_trump(view.trumpCard().index())

{
    // Remap player IDs to their attack order positions
    // In effect, this allows games with reversed player IDs
    // to be mapped to the same information sets
    std::unordered_map<size_t, size_t> remapping;
    for (size_t i = 0; i < view.attackOrder().size(); i++) {
        auto pid = view.attackOrder()[i];
        remapping[pid] = i;
    }

    // Save number of hidden cards in each player's hands
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        auto rid = remapping[pid];
        d_hiddenHands[rid] = view.hiddenHandSize(pid);
    }

    // All cards are initially treated as if they are discarded
    for (auto i = 0; i < MAX_CARDS; i++) {
        d_cardStates[i] = CARD_DISCARDED;
    }
    // Trump card receives a special marking if it's still in the deck
    if (view.deckSize() > 0) {
        d_cardStates[d_trump] = CARD_DECK_BOTTOM;
    }
    // Mark cards that are hidden
    for (auto& card : view.hiddenCards()) {
        d_cardStates[card.index()] = CARD_HIDDEN;
    }
    // Mark which cards belong to which player hands
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        auto rid = remapping[pid];
        for (auto& card : view.visibleHand(pid)) {
            d_cardStates[card.index()] = rid;
        }
    }
    // Mark cards currently used in the attack
    for (auto& card : view.currentAttack()) {
        d_cardStates[card.index()] = CARD_IN_ATTACK;
    }
    // If it is the defender's turn, then save the last attacking card
    if (!view.attackerNext()) {
        auto& card = view.currentAttack().back();
        d_cardStates[card.index()] = CARD_LAST_IN_ATTACK;
    }
    // Mark cards currently used in the defense
    for (auto& card : view.currentDefense()) {
        d_cardStates[card.index()] = CARD_IN_DEFENSE;
    }
    // OPTIMIZATION: if there are only two players in the game and no
    // cards in the deck, this means that all hidden cards must belong to
    // the other player
    if (view.playerCount() == 2 && view.deckSize() == 0) {
        for (size_t rid = 0; rid < d_hiddenHands.size(); rid++) {
            if (d_hiddenHands[rid] > 0) {
                for (auto i = 0; i < MAX_CARDS; i++) {
                    if (d_cardStates[i] == CARD_HIDDEN) {
                        d_cardStates[i] = rid;
                    }
                }
                break;
            }
        }
    }
}

bool CFRMAbstraction::operator==(const CFRMAbstraction& abstraction) const
{
    return d_trump == abstraction.d_trump
        && d_cardStates == abstraction.d_cardStates
        && d_hiddenHands == abstraction.d_hiddenHands;
}

std::ostream& operator<<(std::ostream& os, const CFRMAbstraction& abstraction)
{
    std::vector<Hand> hands(abstraction.d_hiddenHands.size());
    Hand attack;
    Hand defense;
    for (uint8_t i = 0; i < MAX_CARDS; i++) {
        Card card = toCard(i);
        auto state = abstraction.d_cardStates[i];
        if (state == CARD_IN_ATTACK) {
            attack.insert(card);
        } else if (state == CARD_LAST_IN_ATTACK) {
            attack.insert(card);
        } else if (state == CARD_IN_DEFENSE) {
            defense.insert(card);
        } else if (state == CARD_HIDDEN) {
            continue;
        } else if (state == CARD_DISCARDED) {
            continue;
        } else if (state == CARD_DECK_BOTTOM) {
            continue;
        } else {
            hands[state].insert(card);
        }
    }
    for (size_t pid = 0; pid < abstraction.d_hiddenHands.size(); pid++) {
        os << "  P" << pid << " -- " 
            << hands[pid] << " " 
            << static_cast<size_t>(abstraction.d_hiddenHands[pid]) << std::endl;
    }
    return os
        << "  CA -- " << attack << std::endl
        << "  CD -- " << defense << std::endl;
}

}
