#ifndef JESTER_TABULAR_CFRM_HPP
#define JESTER_TABULAR_CFRM_HPP

#include "cfrm_abstraction.hpp"
#include "cfrm_stats.hpp"

#include <mutex>
#include <random>

namespace jester {

class Game;

class TabularCFRM {
private:
    std::mt19937 d_rng;
    std::mutex d_mtx;
    std::unordered_map<CFRMAbstraction, CFRMStats> d_stats;

public:
    TabularCFRM();

    Action sampleStrategy(const std::unordered_map<Action, float>& strategy);
    Action bestAction(const GameView& view);

    void iterate(bool verbose);
    void randomizeSeed();

    template <class Archive>
    void serialize(Archive& archive);

private:
    float train(bool verbose, size_t tpid, const Game& game, const std::vector<float>& reaches);
};

inline void TabularCFRM::randomizeSeed()
{
    d_rng = std::mt19937(std::random_device {}());
}


template <class Archive>
inline void TabularCFRM::serialize(Archive& archive)
{
    archive(d_stats);
}

}

#endif
