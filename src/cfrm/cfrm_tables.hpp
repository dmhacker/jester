#ifndef JESTER_CFRM_TABLES_HPP
#define JESTER_CFRM_TABLES_HPP

#include "cfrm_abstraction.hpp"
#include "cfrm_stats.hpp"

#include <mutex>
#include <random>
#include <thread>

namespace jester {

class Game;

class CFRMTables {
private:
    std::unordered_map<CFRMAbstraction, CFRMStats> d_stats;
    std::mt19937 d_rng;
    std::mutex d_mtx;

public:
    CFRMTables();

    Action bestAction(const GameView& view, bool verbose);

    Action sample(const std::unordered_map<Action, float>& profile, std::mt19937& rng);
    float train(size_t tpid, const Game& game, std::mt19937& rng);

    template <class Archive>
    void serialize(Archive& archive);

    std::mutex& mutex();
};

template <class Archive>
inline void CFRMTables::serialize(Archive& archive)
{
    archive(d_stats);
}
    
inline std::mutex& CFRMTables::mutex() {
    return d_mtx;
}

}

#endif
