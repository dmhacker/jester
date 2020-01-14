#include <catch.hpp>

#include "../src/rules/game_state.hpp"
#include "../src/constants.hpp"

using namespace jester;

TEST_CASE("Game resets work correctly")
{
    std::mt19937 rng;
    size_t deck_size = (Constants::instance().MAX_RANK - Constants::instance().MIN_RANK + 1) * 4;
    for (size_t i = 2; i < 6; i++) {
        GameState state(i, rng);
        REQUIRE(state.deck().size() == deck_size - Constants::instance().HAND_SIZE * i);
        REQUIRE(state.hiddenCards().size() == deck_size - 1);
    }
}
