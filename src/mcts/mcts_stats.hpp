#ifndef JESTER_MCTS_STATS_HPP
#define JESTER_MCTS_STATS_HPP

#include <cstddef>
#include <iostream>

namespace jester {

class MCTSStats {
private:
    size_t d_playouts;
    float d_reward;

public:
    MCTSStats();

    size_t playouts() const;
    float reward() const;
    float rewardRatio() const;

    void addStats(const MCTSStats& stats);
    void addReward(float reward);
    void incrementPlayouts();

private:
    friend std::ostream& operator<<(std::ostream& os, const MCTSStats& stats);
};

inline size_t MCTSStats::playouts() const
{
    return d_playouts;
}

inline float MCTSStats::reward() const
{
    return d_reward;
}

inline float MCTSStats::rewardRatio() const
{
    if (d_playouts == 0) {
        return 0;
    }
    return d_reward / d_playouts;
}

inline void MCTSStats::addStats(const MCTSStats& stats)
{
    d_reward += stats.d_reward;
    d_playouts += stats.d_playouts;
}

inline void MCTSStats::addReward(float reward)
{
    d_reward += reward;
}

inline void MCTSStats::incrementPlayouts()
{
    d_playouts++;
}

}

#endif
