#include <jester/mcts/mcts_stats.hpp>

namespace jester {

MCTSStats::MCTSStats()
    : d_playouts(0)
    , d_reward(0)
{
}

std::ostream& operator<<(std::ostream& os, const MCTSStats& stats)
{
    return os 
        << stats.d_reward << " / "
        << stats.d_playouts << " / "
        << stats.rewardRatio();
}

}
