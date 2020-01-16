#include "cfrm_key.hpp"
#include "../constants.hpp"
#include "../logging.hpp"
#include "../rules/game_view.hpp"

namespace jester {

namespace {
    size_t shiftedIndex(const Card& card)
    {
        return card.index() - Constants::instance().MIN_RANK * 4;
    }

    Card fromIndex(uint8_t index)
    {
        return Card(index / 4 + Constants::instance().MIN_RANK,
            static_cast<Suit>(index % 4));
    }
}

constexpr static uint8_t MAX_CARDS = 36;
constexpr static uint8_t CARD_HIDDEN = 6;
constexpr static uint8_t CARD_DISCARDED = 7;
constexpr static uint8_t CARD_DECK_BOTTOM = 8;
constexpr static uint8_t CARD_IN_ATTACK = 9;
constexpr static uint8_t CARD_IN_DEFENSE = 10;
constexpr static uint8_t CARD_LAST_IN_ATTACK = 11;

CFRMKey::CFRMKey(const GameView& view)
    : d_cardStates(MAX_CARDS)
    , d_hiddenHands(view.playerCount())
    , d_trump(shiftedIndex(view.trumpCard()))

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
    for (auto cidx = 0; cidx < MAX_CARDS; cidx++) {
        d_cardStates[cidx] = CARD_DISCARDED;
    }
    // Trump card receives a special marking if it's still in the deck
    if (view.deckSize() > 0) {
        d_cardStates[d_trump] = CARD_DECK_BOTTOM;
    }
    // Mark cards that are hidden
    for (auto& card : view.hiddenCards()) {
        d_cardStates[shiftedIndex(card)] = CARD_HIDDEN;
    }
    // Mark which cards belong to which player hands
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        auto rid = remapping[pid];
        for (auto& card : view.visibleHand(pid)) {
            d_cardStates[shiftedIndex(card)] = rid;
        }
    }
    // Mark cards currently used in the attack
    for (auto& card : view.currentAttack()) {
        d_cardStates[shiftedIndex(card)] = CARD_IN_ATTACK;
    }
    // If it is the defender's turn, then save the last attacking card
    if (!view.attackerNext()) {
        auto& card = view.currentAttack().back();
        d_cardStates[shiftedIndex(card)] = CARD_LAST_IN_ATTACK;
    }
    // Mark cards currently used in the defense
    for (auto& card : view.currentDefense()) {
        d_cardStates[shiftedIndex(card)] = CARD_IN_DEFENSE;
    }
    // OPTIMIZATION: if there are no cards in the deck and only one
    // hand with hidden cards, then that hand must possess all hidden cards
    if (view.deckSize() == 0) {
        size_t holder = 0;
        size_t nonzero_hands = 0;
        for (size_t rid = 0; rid < d_hiddenHands.size(); rid++) {
            if (d_hiddenHands[rid] > 0) {
                nonzero_hands++;
                holder = rid;
            }
        }
        if (nonzero_hands == 1) {
            d_hiddenHands[holder] = 0;
            for (size_t cidx = 0; cidx < MAX_CARDS; cidx++) {
                if (d_cardStates[cidx] == CARD_HIDDEN) {
                    d_cardStates[cidx] = holder;
                }
            }
        }
    }
}

bool CFRMKey::operator==(const CFRMKey& abstraction) const
{
    return d_trump == abstraction.d_trump
        && d_cardStates == abstraction.d_cardStates
        && d_hiddenHands == abstraction.d_hiddenHands;
}

std::ostream& operator<<(std::ostream& os, const CFRMKey& key)
{
    std::vector<Hand> hands(key.d_hiddenHands.size());
    Hand attack;
    Hand defense;
    for (uint8_t i = 0; i < MAX_CARDS; i++) {
        Card card = fromIndex(i);
        auto state = key.d_cardStates[i];
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
    for (size_t pid = 0; pid < key.d_hiddenHands.size(); pid++) {
        os << "  P" << pid << " -- "
           << hands[pid] << " "
           << static_cast<size_t>(key.d_hiddenHands[pid]) << std::endl;
    }
    return os
        << "  CA -- " << attack << std::endl
        << "  CD -- " << defense << std::endl;
}

}
