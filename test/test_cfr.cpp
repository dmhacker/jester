#include <catch.hpp>
#include <cstdio>
#include <fstream>

#include <game/constants.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <cfr/cfr_engine.hpp>
#include <cfr/unordered_cfr_table.hpp>

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
        auto it = cfr.table()->find(key);
        REQUIRE(it != nullptr);
        auto& entry = *it;
        REQUIRE(entry.averageProfile().size() == state.nextActions().size());
        REQUIRE(entry.currentProfile().size() == state.nextActions().size());
        REQUIRE(validDistribution(entry.averageProfile()));
        REQUIRE(validDistribution(entry.currentProfile()));
    }
    for (auto& action : state.nextActions()) {
        GameState cpy(state);
        cpy.playAction(action);
        auto key = CFRInfoSet(cpy.currentPlayerView());
        auto it = cfr.table()->find(key);
        REQUIRE(it != nullptr);
        auto& entry = *it;
        REQUIRE(validDistribution(entry.averageProfile()));
        REQUIRE(validDistribution(entry.currentProfile()));
    }
    Constants::instance().MAX_RANK = 14;
}
