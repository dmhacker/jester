#ifndef JESTER_ISMCTS_PLAYER_HPP
#define JESTER_ISMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class ISMCTSPlayer : public Player {
private:
    bool d_verbose;
    size_t d_workerCount;
    std::chrono::milliseconds d_timeLimit;

public:
    ISMCTSPlayer(bool verbose = false, size_t workers = 4,
        const std::chrono::milliseconds& time_limit = std::chrono::milliseconds(8000));
    Action nextAction(const GameView& view);
};

}

#endif
