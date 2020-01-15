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
    std::mutex d_mtx;
    bool d_verbose;

public:
    CFRMTable(bool verbose);

    Action bestAction(const GameView& view, std::mt19937& rng);
    Action sampleAction(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng);
    float train(size_t tpid, const GameState& state, std::mt19937& rng);

    template <class Archive>
    void serialize(Archive& archive);

    std::mutex& mutex();
    const std::unordered_map<CFRMKey, CFRMStats>& table() const;
};

template <class Archive>
inline void CFRMTable::serialize(Archive& archive)
{
    archive(d_table);
}

inline const std::unordered_map<CFRMKey, CFRMStats>& CFRMTable::table() const {
    return d_table;
}
     
inline std::mutex& CFRMTable::mutex() {
    return d_mtx;
}   

}

#endif
