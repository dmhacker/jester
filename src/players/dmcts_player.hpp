#ifndef JESTER_DMCTS_PLAYER_HPP
#define JESTER_DMCTS_PLAYER_HPP

#include "player.hpp"

#include <random>

namespace jester {

class DMCTSPlayer : public Player {
private:
    size_t d_determinizationCount;
    size_t d_workerCount;
    std::chrono::milliseconds d_timeLimit;

public:
    DMCTSPlayer(size_t determinizations, size_t workers,
        const std::chrono::milliseconds& time_limit);
    Action nextAction(const GameView& view);
};

}

#endif
