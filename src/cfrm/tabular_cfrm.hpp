#ifndef JESTER_TABULAR_CFRM_HPP
#define JESTER_TABULAR_CFRM_HPP

#include "cfrm_abstraction.hpp"
#include "cfrm_stats.hpp"

#include <mutex>
#include <random>
#include <thread>

namespace jester {

class Game;

class TabularCFRM {
private:
    std::unordered_map<CFRMAbstraction, CFRMStats> d_stats;
    std::vector<std::mt19937> d_rngs;
    std::mutex d_mtx;

public:
    TabularCFRM();

    Action bestAction(const GameView& view, bool verbose);

    void iterate();

    template <class Archive>
    void serialize(Archive& archive);

private:
    Action sample(const std::unordered_map<Action, float>& profile, std::mt19937& rng);
    float train(size_t tpid, const Game& game, std::mt19937& rng);
};

template <class Archive>
inline void TabularCFRM::serialize(Archive& archive)
{
    archive(d_stats);
}

}

#endif
