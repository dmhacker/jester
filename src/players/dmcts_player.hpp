#ifndef JESTER_DMCTS_PLAYER_HPP
#define JESTER_DMCTS_PLAYER_HPP

#include <random>

#include "player.hpp"

namespace jester {

class DMCTSPlayer : public Player {
public:
    Action attack(const GameView& view, std::chrono::milliseconds time_limit);
    Action defend(const GameView& view, std::chrono::milliseconds time_limit);
};

}

#endif
