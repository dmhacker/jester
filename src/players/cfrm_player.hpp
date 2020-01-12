#ifndef JESTER_CFRM_PLAYER_HPP
#define JESTER_CFRM_PLAYER_HPP

#include <random>

#include "player.hpp"
#include "../cfrm/cfrm_storage.hpp"

namespace jester {

class CFRMPlayer : public Player {
private:
    CFRMStorage d_cfrm;
    bool d_verbose;

public:
    CFRMPlayer(bool verbose);

    Action nextAction(const GameView& view);
};

}

#endif
