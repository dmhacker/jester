#include "compressed_view.hpp"

#include <cstring>

namespace jester {

const static uint8_t CARD_IN_ATTACK = 6;
const static uint8_t CARD_IN_DEFENSE = 7;
const static uint8_t CARD_HIDDEN = 8;
const static uint8_t CARD_DISCARDED = 9;
const static uint8_t CARD_DECK_BOTTOM = 10;

CompressedView::CompressedView(const GameView& view)
    : d_trump(view.trumpCard().index())
    , d_attacking(view.attackerNext())
{
    std::unordered_map<size_t, size_t> remapping;
    for (size_t i = 0; i < view.attackOrder().size(); i++) {
        auto pid = view.attackOrder()[i];
        remapping[pid] = i;
    }

    std::memset(d_hiddenHands, 0, MAX_PLAYERS);
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        d_hiddenHands[pid] = view.hiddenHandSize(pid);
    }
   
    std::memset(d_cardStates, CARD_DISCARDED, MAX_CARDS);
    d_cardStates[d_trump] = CARD_DECK_BOTTOM;
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        for (auto& card : view.visibleHand(pid)) {
            d_cardStates[card.index()] = pid;
        }
    }
    for (auto& card : view.currentAttack()) {
        d_cardStates[card.index()] = CARD_IN_ATTACK;
    }
    for (auto& card : view.currentDefense()) {
        d_cardStates[card.index()] = CARD_IN_DEFENSE;
    }
    for (auto& card : view.hiddenCards()) {
        d_cardStates[card.index()] = CARD_HIDDEN;
    }
}

bool CompressedView::operator==(const CompressedView& cv) const
{
    return d_trump == cv.d_trump 
        && d_attacking == cv.d_attacking
        && std::memcmp(d_cardStates, cv.d_cardStates, MAX_CARDS) == 0
        && std::memcmp(d_hiddenHands, cv.d_hiddenHands, MAX_PLAYERS) == 0;
}

}
