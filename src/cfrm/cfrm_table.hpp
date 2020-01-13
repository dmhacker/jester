#ifndef JESTER_CFRM_TABLE_HPP
#define JESTER_CFRM_TABLE_HPP

#include "cfrm_key.hpp"
#include "cfrm_stats.hpp"

#include <mutex>
#include <random>
#include <thread>

namespace jester {

class Game;

class CFRMTable {
private:
    std::unordered_map<CFRMKey, CFRMStats> d_table;
    std::mt19937 d_rng;
    std::mutex d_mtx;

public:
    CFRMTable();

    Action bestAction(const GameView& view, bool verbose);

    Action sample(const std::unordered_map<Action, float>& profile, std::mt19937& rng);
    float train(size_t tpid, const Game& game, std::mt19937& rng);

    template <class Archive>
    void serialize(Archive& archive);

    std::mutex& mutex();
};

template <class Archive>
inline void CFRMTable::serialize(Archive& archive)
{
    archive(d_table);
}
    
inline std::mutex& CFRMTable::mutex() {
    return d_mtx;
}

}

#endif
