#include <catch.hpp>

#include "../src/constants.hpp"
#include "../src/players/greedy_player.hpp"
#include "../src/rules/game_state.hpp"
#include "../src/rules/game_view.hpp"

using namespace jester;

static std::mt19937 rng(123);

TEST_CASE("Game view reflects current game state")
{
    GreedyPlayer player;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        while (!state.finished()) {
            for (size_t pid = 0; pid < pcnt; pid++) {
                GameView view(state, pid);
                REQUIRE(view.playerId() == pid);
                REQUIRE(view.firstMove() == state.firstMove());
                REQUIRE(!view.finished());
                REQUIRE(view.attackerNext() == state.attackerNext());
                REQUIRE(view.playerCount() == pcnt);
                REQUIRE(view.deckSize() == state.deck().size());
                REQUIRE(view.nextActions() == state.nextActions());
                REQUIRE(view.winOrder() == state.winOrder());
                REQUIRE(view.attackOrder() == state.attackOrder());
                REQUIRE(view.hiddenCards() == state.hiddenCards());
                REQUIRE(view.currentAttack() == state.currentAttack());
                REQUIRE(view.currentDefense() == state.currentDefense());
                REQUIRE(view.trumpCard() == state.trumpCard());
                REQUIRE(view.trumpSuit() == state.trumpSuit());
                REQUIRE(view.attackerId() == state.attackerId());
                REQUIRE(view.defenderId() == state.defenderId());
            }
            auto action = player.nextAction(
                state.currentPlayerView());
            state.validateAction(action);
            state.playAction(action);
        }
    }
}

TEST_CASE("Hands are hidden correctly")
{
    GreedyPlayer player;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        while (!state.finished()) {
            for (size_t pid = 0; pid < pcnt; pid++) {
                GameView view(state, pid);
                REQUIRE(view.hand() == state.hand(pid));
                REQUIRE(view.visibleHand(pid) == state.hand(pid));
                for (size_t pid2 = 0; pid2 < pcnt; pid2++) {
                    REQUIRE(view.visibleHand(pid2).size()
                            + view.hiddenHandSize(pid2)
                        == state.hand(pid2).size());
                    REQUIRE(view.handSize(pid2)
                        == state.hand(pid2).size());
                }
                for (size_t pid2 = 0; pid2 < pcnt; pid2++) {
                    if (pid != pid2) {
                        size_t hcnt = 0;
                        for (auto& card : state.hand(pid2)) {
                            if (state.hiddenCards().find(card) 
                                    == state.hiddenCards().end()) {
                                auto vhand = view.visibleHand(pid2);
                                REQUIRE(vhand.find(card) != vhand.end());
                            }
                            else {
                                hcnt++;
                            }
                        }
                        REQUIRE(view.hiddenHandSize(pid2) == hcnt);
                    }
                }
            }
            auto action = player.nextAction(
                state.currentPlayerView());
            state.validateAction(action);
            state.playAction(action);
        }
    }
}
