#include <catch.hpp>

#include "../src/cfrm/cfrm_key.hpp"
#include "../src/cfrm/cfrm_table.hpp"
#include "../src/constants.hpp"
#include "../src/rules/game_state.hpp"
#include "../src/rules/game_view.hpp"

using namespace jester;

static std::mt19937 rng(123);

namespace {

bool validDistribution(const std::vector<float>& distribution)
{
    float sum = 0;
    for (auto prob : distribution) {
        sum += prob;
    }
    return fabsf(sum - 1.f) < 1e-6;
}

}

TEST_CASE("2-player CFRM trains correctly on reduced game")
{
    Constants::instance().MAX_RANK = 6;
    size_t pcnt = 2;
    GameState state(pcnt, rng);
    CFRMTable table(false);
    table.train(0, state, rng);
    table.train(1, state, rng);
    {
        auto key = CFRMKey(state.currentPlayerView());
        auto it = table.table().find(key);
        REQUIRE(it != table.table().end());
        auto& stats = it->second;
        REQUIRE(stats.averageProfile().size() == state.nextActions().size());
        REQUIRE(stats.currentProfile().size() == state.nextActions().size());
    }
    for (auto& action : state.nextActions()) {
        GameState cpy(state);
        cpy.playAction(action);
        auto key = CFRMKey(cpy.currentPlayerView());
        auto it = table.table().find(key);
        REQUIRE(it != table.table().end());
    }
    for (auto it : table.table()) {
        auto& stats = it.second;
        REQUIRE(validDistribution(stats.averageProfile()));
        REQUIRE(validDistribution(stats.currentProfile()));
    }
    Constants::instance().MAX_RANK = 14;
}
