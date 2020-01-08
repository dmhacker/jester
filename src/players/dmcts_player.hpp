#ifndef JESTER_DMCTS_PLAYER_HPP
#define JESTER_DMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class DMCTSPlayer : public Player {
private:
    bool d_verbose;
    size_t d_determinizationCount;
    size_t d_workerCount;
    std::chrono::milliseconds d_timeLimit;

public:
    DMCTSPlayer(bool verbose = false,
        size_t determinizations = 4, size_t workers = 4,
        const std::chrono::milliseconds& time_limit = std::chrono::milliseconds(8000));
    Action nextAction(const GameView& view);
};

}

#endif
