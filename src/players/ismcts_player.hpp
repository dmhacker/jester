#ifndef JESTER_ISMCTS_PLAYER_HPP
#define JESTER_ISMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class ISMCTSPlayer : public Player {
private:
    size_t d_workerCount;
    std::chrono::milliseconds d_timeLimit;

public:
    ISMCTSPlayer(size_t workers,
        const std::chrono::milliseconds& time_limit);
    Action nextAction(const GameView& view);
};

}

#endif
