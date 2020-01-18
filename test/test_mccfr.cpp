#include <catch.hpp>
#include <fstream>
#include <cstdio>

#include "../src/mccfr/mccfr_engine.hpp"
#include "../src/mccfr/mccfr_table.hpp"
#include "../src/constants.hpp"
#include "../src/game/game_state.hpp"
#include "../src/game/game_view.hpp"

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

TEST_CASE("MCCFR trains correctly on reduced 2-player game")
{
    Constants::instance().MAX_RANK = 6;
    size_t pcnt = 2;
    GameState state(pcnt, rng);
    MCCFRTable table;
    table.train(0, state, rng);
    table.train(1, state, rng);
    {
        auto key = MCCFRInfoSet(state.currentPlayerView());
        auto it = table.table().find(key);
        REQUIRE(it != table.table().end());
        auto& stats = it->second;
        REQUIRE(stats.averageProfile().size() == state.nextActions().size());
        REQUIRE(stats.currentProfile().size() == state.nextActions().size());
    }
    for (auto& action : state.nextActions()) {
        GameState cpy(state);
        cpy.playAction(action);
        auto key = MCCFRInfoSet(cpy.currentPlayerView());
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

TEST_CASE("MCCFR can be saved and loaded from binary file")
{
    Constants::instance().MAX_RANK = 6;
    size_t pcnt = 2;
    size_t tbsz = 0;
    {
        MCCFREngine env("test.bin");
        GameState state(pcnt, rng);
        env.strategy().train(0, state, rng);
        tbsz = env.strategy().table().size();
        REQUIRE(tbsz > 0);
        env.save();
    }
    REQUIRE(std::ifstream("test.bin"));
    {
        MCCFREngine env("test.bin");
        REQUIRE(env.strategy().table().size() == tbsz);
    }
    std::remove("test.bin");    
    REQUIRE(!std::ifstream("test.bin"));
    Constants::instance().MAX_RANK = 14;
}
