#include <catch.hpp>

#include <jester/cfr/unordered_cfr_table.hpp>
#include <jester/game/constants.hpp>
#include <jester/game/game_state.hpp>
#include <jester/players/cfr_player.hpp>
#include <jester/players/dmcts_player.hpp>
#include <jester/players/greedy_player.hpp>
#include <jester/players/ismcts_player.hpp>
#include <jester/players/minimal_player.hpp>
#include <jester/players/random_player.hpp>

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

TEST_CASE("Empty CFR player works correctly")
{
    CFREngine engine(stda::make_erased<UnorderedCFRTable>());
    CFRPlayer player(engine);
    GreedyPlayer opponent;
    for (size_t pcnt = Constants::instance().MIN_PLAYERS;
         pcnt <= Constants::instance().MAX_PLAYERS; pcnt++) {
        GameState state(pcnt, rng);
        playGame(player, opponent, state);
    }
}

TEST_CASE("Trained CFR player works correctly")
{
    Constants::instance().MAX_RANK = 7;
    CFREngine engine(stda::make_erased<UnorderedCFRTable>());
    CFRPlayer player(engine);
    GreedyPlayer opponent;
    GameState state(2, rng);
    engine.train(0, state, rng);
    engine.train(1, state, rng);
    playGame(player, opponent, state);
    Constants::instance().MAX_RANK = 14;
}
