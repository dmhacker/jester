#include <catch.hpp>

#include "../src/constants.hpp"
#include "../src/players/greedy_player.hpp"
#include "../src/game/game_state.hpp"

using namespace jester;

static std::mt19937 rng(123);

TEST_CASE("Game is in a reset state")
{
    size_t card_count = (Constants::instance().MAX_RANK
                            - Constants::instance().MIN_RANK + 1)
        * 4;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        // Check sizes for common methods
        REQUIRE(state.playerCount() == pcnt);
        REQUIRE(state.deck().size()
            == card_count - Constants::instance().HAND_SIZE * pcnt);
        REQUIRE(state.hiddenCards().size()
            == card_count - 1);
        REQUIRE(state.currentPlayerId() == 0);
        REQUIRE(state.attackerId() == 0);
        REQUIRE(state.defenderId() == 1);
        REQUIRE(state.turn() == 0);
        REQUIRE(state.attackerNext());
        REQUIRE(state.currentAttack().empty());
        REQUIRE(state.currentDefense().empty());
        REQUIRE(state.winOrder().empty());
        REQUIRE(state.attackOrder().size() == pcnt);
        REQUIRE(!state.finished());
        if (!state.deck().empty()) {
            REQUIRE(state.deck().back() == state.trumpCard());
        }
        REQUIRE(state.trumpSuit() == state.trumpCard().suit());
        REQUIRE(state.nextActions().size() == state.hand(0).size());
        REQUIRE(std::find(state.nextActions().begin(),
                    state.nextActions().end(),
                    Action())
            == state.nextActions().end());
        // Check to make sure all cards are distributed uniquely
        std::unordered_set<Card> cards;
        for (size_t pid = 0; pid < pcnt; pid++) {
            auto& hand = state.hand(pid);
            for (auto& card : hand) {
                REQUIRE(cards.find(card) == cards.end());
                cards.insert(card);
            }
        }
        for (auto& card : state.deck()) {
            REQUIRE(cards.find(card) == cards.end());
            cards.insert(card);
        }
        REQUIRE(cards.size() == card_count);
    }
}

TEST_CASE("Game is in a defending state")
{
    size_t card_count = (Constants::instance().MAX_RANK
                            - Constants::instance().MIN_RANK + 1)
        * 4;
    GreedyPlayer player;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        // Playing 1 action results in a defender's turn
        for (size_t a = 0; a < 1; a++) {
            auto action = player.nextAction(
                state.currentPlayerView());
            state.validateAction(action);
            REQUIRE(std::find(state.nextActions().begin(),
                        state.nextActions().end(), action)
                != state.nextActions().end());
            state.playAction(action);
        }
        REQUIRE(state.playerCount() == pcnt);
        REQUIRE(state.currentPlayerId() == state.defenderId());
        REQUIRE(state.currentAttack().size() == 1);
        REQUIRE(state.currentDefense().size() == 0);
        REQUIRE(!state.attackerNext());
        REQUIRE(!state.finished());
        REQUIRE(!state.nextActions().empty());
        REQUIRE(state.winOrder().empty());
        REQUIRE(state.attackOrder().size() == pcnt);
        REQUIRE(state.deck().size()
            == card_count - Constants::instance().HAND_SIZE * pcnt);
        if (!state.deck().empty()) {
            REQUIRE(state.deck().back() == state.trumpCard());
        }
        for (size_t pid = 0; pid < pcnt; pid++) {
            REQUIRE(std::find(state.attackOrder().begin(),
                        state.attackOrder().end(), pid)
                != state.attackOrder().end());
        }
    }
}

TEST_CASE("Game is in an attacking state")
{
    GreedyPlayer player;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        // Playing 2 actions results in an attacker's turn
        for (size_t a = 0; a < 2; a++) {
            auto action = player.nextAction(
                state.currentPlayerView());
            state.validateAction(action);
            REQUIRE(std::find(state.nextActions().begin(),
                        state.nextActions().end(), action)
                != state.nextActions().end());
            state.playAction(action);
        }
        REQUIRE(state.playerCount() == pcnt);
        REQUIRE(state.currentPlayerId() == state.attackerId());
        REQUIRE(state.currentAttack().size() == state.currentDefense().size());
        REQUIRE(state.attackerNext());
        REQUIRE(!state.finished());
        REQUIRE(!state.nextActions().empty());
        REQUIRE(state.winOrder().empty());
        REQUIRE(state.attackOrder().size() == pcnt);
        if (!state.deck().empty()) {
            REQUIRE(state.deck().back() == state.trumpCard());
        }
        for (size_t pid = 0; pid < pcnt; pid++) {
            REQUIRE(std::find(state.attackOrder().begin(),
                        state.attackOrder().end(), pid)
                != state.attackOrder().end());
        }
    }
}

TEST_CASE("Game is in a finished state")
{
    GreedyPlayer player;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        while (!state.finished()) {
            // Check to make sure all players are accounted for
            REQUIRE(state.attackOrder().size() + state.winOrder().size() == pcnt);
            // Check to make sure that player hands are being replenished
            if (!state.deck().empty()) {
                if (state.currentAttack().empty() && state.currentDefense().empty()) {
                    for (size_t pid = 0; pid < pcnt; pid++) {
                        REQUIRE(state.hand(pid).size() >= Constants::instance().HAND_SIZE);
                    }
                }
                REQUIRE(state.hand(state.attackerId()).size()
                        + state.currentAttack().size()
                    >= Constants::instance().HAND_SIZE);
                REQUIRE(state.hand(state.defenderId()).size()
                        + state.currentDefense().size()
                    >= Constants::instance().HAND_SIZE);
            }
            // Check to make sure current attack and defense have correct sizes
            if (state.attackerNext()) {
                REQUIRE(state.currentAttack().size() == state.currentDefense().size());
            } else {
                REQUIRE(state.currentAttack().size() == state.currentDefense().size() + 1);
            }
            auto action = player.nextAction(
                state.currentPlayerView());
            state.validateAction(action);
            REQUIRE(std::find(state.nextActions().begin(),
                        state.nextActions().end(), action)
                != state.nextActions().end());
            state.playAction(action);
        }
        REQUIRE(state.playerCount() == pcnt);
        REQUIRE(state.attackOrder().empty());
        REQUIRE(state.nextActions().empty());
        REQUIRE(state.winOrder().size() == pcnt);
        REQUIRE(state.deck().empty());
        for (size_t pid = 0; pid < pcnt; pid++) {
            REQUIRE(std::find(state.winOrder().begin(),
                        state.winOrder().end(), pid)
                != state.winOrder().end());
        }
    }
}

TEST_CASE("Invalid yield is caught by validate")
{
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        // Player should not be able to yield (first attack)
        try {
            state.validateAction(Action());
            REQUIRE(false);
        } catch (std::logic_error& le) {
            REQUIRE(!std::string(le.what()).empty());
        }
    }
}
