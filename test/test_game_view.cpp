#include <catch.hpp>

#include "../src/rules/game_state.hpp"
#include "../src/rules/game_view.hpp"
#include "../src/constants.hpp"

using namespace jester;

TEST_CASE("Game view has correct deck size")
{
    std::mt19937 rng;
    for (size_t i = 2; i < 6; i++) {
        GameState state(i, rng);
        REQUIRE(state.deck().size() == state.currentPlayerView().deckSize());
    }
}
