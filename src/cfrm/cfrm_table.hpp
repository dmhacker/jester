#ifndef JESTER_CFRM_TABLE_HPP
#define JESTER_CFRM_TABLE_HPP

#include "cfrm_key.hpp"
#include "cfrm_stats.hpp"

#include <mutex>
#include <random>
#include <thread>

#include <cereal/types/unordered_map.hpp>

namespace jester {

class GameState;

class CFRMTable {
private:
    std::unordered_map<CFRMKey, CFRMStats> d_table;
    std::mt19937 d_rng;
    std::mutex d_mtx;

public:
    CFRMTable();

    Action bestAction(const GameView& view, bool verbose);

    Action sample(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng);
    float train(size_t tpid, const GameState& game, std::mt19937& rng);

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
