#include "cfrm_player.hpp"
#include "../logging.hpp"

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

CFRMPlayer::CFRMPlayer()
    : d_rng(std::random_device {}())
{
}

Action CFRMPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }
    if (bots_logger != nullptr) {
        bots_logger->info("CFRM lookup has been started for P{}.",
            view.playerId());
    }
    return d_cfrm.strategy().bestAction(view, d_rng);
}

}
