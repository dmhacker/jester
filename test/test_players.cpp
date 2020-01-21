#include <catch.hpp>

#include <game/constants.hpp>
#include <game/game_state.hpp>
#include <players/dmcts_player.hpp>
#include <players/greedy_player.hpp>
#include <players/ismcts_player.hpp>
#include <players/minimal_player.hpp>
#include <players/random_player.hpp>
#include <players/mccfr_player.hpp>

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

TEST_CASE("MCCFR player works correctly")
{
    MCCFRPlayer player;
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}
