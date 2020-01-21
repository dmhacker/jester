#ifndef JESTER_MCCFR_TABLE_HPP
#define JESTER_MCCFR_TABLE_HPP

#include "mccfr_infoset.hpp"
#include "mccfr_entry.hpp"

#include <mutex>
#include <random>
#include <thread>

#include <cereal/types/unordered_map.hpp>

namespace jester {

class GameState;

class MCCFRTable {
private:
    std::unordered_map<MCCFRInfoSet, MCCFREntry> d_table;
    std::mutex d_mtx;

public:
    Action bestAction(const GameView& view, std::mt19937& rng);
    Action sampleAction(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng);
    float train(size_t tpid, const GameState& state, std::mt19937& rng);

    template <class Archive>
    void serialize(Archive& archive);

    std::mutex& mutex();
    const std::unordered_map<MCCFRInfoSet, MCCFREntry>& table() const;
};

template <class Archive>
inline void MCCFRTable::serialize(Archive& archive)
{
    archive(d_table);
}

inline const std::unordered_map<MCCFRInfoSet, MCCFREntry>& MCCFRTable::table() const {
    return d_table;
}
     
inline std::mutex& MCCFRTable::mutex() {
    return d_mtx;
}   

}

#endif
