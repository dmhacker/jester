#include "cfrm_player.hpp"

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

CFRMPlayer::CFRMPlayer(bool verbose)
    : d_verbose(verbose)
{
}

Action CFRMPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }
    return d_cfrm.strategy().bestAction(view, d_verbose);
}

}
