#include "mccfr_infoset.hpp"
#include "../constants.hpp"
#include "../logging.hpp"
#include "../game/game_view.hpp"

namespace jester {

namespace {
    size_t shiftedIndex(const Card& card)
    {
        return card.index() - Constants::instance().MIN_RANK * 4;
    }
}

constexpr static uint8_t MAX_CARDS = 36;
constexpr static uint8_t CARD_HIDDEN = 6;
constexpr static uint8_t CARD_DISCARDED = 7;
constexpr static uint8_t CARD_IN_PLAY = 8;
constexpr static uint8_t CARD_LAST_IN_PLAY = 9;

MCCFRInfoSet::MCCFRInfoSet(const GameView& view)
    : d_cardStates(MAX_CARDS)
    , d_hiddenHands(view.playerCount())
    , d_trump(view.trumpCard().suit())

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
        d_cardStates[shiftedIndex(card)] = CARD_IN_PLAY;
    }
    // If it is the defender's turn, then save the last attacking card
    if (!view.attackerNext()) {
        auto& card = view.currentAttack().back();
        d_cardStates[shiftedIndex(card)] = CARD_LAST_IN_PLAY;
    }
    // Mark cards currently used in the defense
    for (auto& card : view.currentDefense()) {
        d_cardStates[shiftedIndex(card)] = CARD_IN_PLAY;
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

bool MCCFRInfoSet::operator==(const MCCFRInfoSet& is) const
{
    return d_trump == is.d_trump
        && d_cardStates == is.d_cardStates
        && d_hiddenHands == is.d_hiddenHands;
}

}
