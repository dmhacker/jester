#include <catch.hpp>

#include <cstdio>
#include <fstream>

#include <jester/cfr/cfr_engine.hpp>
#include <jester/cfr/unordered_cfr_table.hpp>
#include <jester/game/constants.hpp>
#include <jester/game/game_state.hpp>
#include <jester/game/game_view.hpp>

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

TEST_CASE("CFR trains correctly on reduced 2-player game")
{
    Constants::instance().MAX_RANK = 7;
    size_t pcnt = 2;
    GameState state(pcnt, rng);
    CFREngine cfr(stda::make_erased<UnorderedCFRTable>());
    cfr.train(0, state, rng);
    cfr.train(1, state, rng);
    REQUIRE(cfr.table()->size() >= state.nextActions().size());
    {
        auto key = CFRInfoSet(state.currentPlayerView());
        auto pit = cfr.table()->findProfile(key, state.nextActions().size());
        REQUIRE(pit != nullptr);
        auto& profile = *pit;
        REQUIRE(profile.bestResponse().size() == state.nextActions().size());
        REQUIRE(validDistribution(profile.bestResponse()));
    }
    for (auto& action : state.nextActions()) {
        GameState cpy(state);
        cpy.playAction(action);
        auto key = CFRInfoSet(cpy.currentPlayerView());
        auto pit = cfr.table()->findProfile(key, state.nextActions().size());
        REQUIRE(pit != nullptr);
        auto& profile = *pit;
        REQUIRE(validDistribution(profile.bestResponse()));
    }
    REQUIRE(cfr.table()->hits() > 0);
    REQUIRE(cfr.table()->misses() > 0);
    Constants::instance().MAX_RANK = 14;
}
