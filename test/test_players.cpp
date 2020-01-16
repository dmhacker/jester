#include <catch.hpp>

#include "../src/constants.hpp"
#include "../src/players/cfrm_player.hpp"
#include "../src/players/dmcts_player.hpp"
#include "../src/players/greedy_player.hpp"
#include "../src/players/ismcts_player.hpp"
#include "../src/players/minimal_player.hpp"
#include "../src/players/random_player.hpp"
#include "../src/rules/game_state.hpp"

using namespace jester;

static std::mt19937 rng(123);

void playGame(Player& tester, Player& opponent, GameState& state)
{
    while (!state.finished()) {
        auto& player = state.currentPlayerId() == 0 ? tester : opponent;
        auto action = player.nextAction(
            state.currentPlayerView());
        state.validateAction(action);
        REQUIRE(std::find(state.nextActions().begin(),
                    state.nextActions().end(), action)
            != state.nextActions().end());
        state.playAction(action);
    }
}

TEST_CASE("Minimal player works correctly")
{
    MinimalPlayer player;
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("Random player works correctly")
{
    RandomPlayer player;
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("Greedy player works correctly")
{
    GreedyPlayer player;
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("CFRM player works correctly")
{
    CFRMPlayer player(false);
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("DMCTS player works correctly")
{
    DMCTSPlayer player(2, 1, std::chrono::milliseconds(10));
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("ISMCTS player works correctly")
{
    ISMCTSPlayer player(1, std::chrono::milliseconds(10));
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}
