#include <players/cfr_player.hpp>
#include <utils/logging.hpp>

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

CFRPlayer::CFRPlayer(CFREngine& engine)
    : d_cfr(engine)
    , d_rng(std::random_device {}())
{
}

Action CFRPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }
    if (bots_logger != nullptr) {
        bots_logger->info("CFR lookup has been started for P{}.",
            view.playerId());
    }
    return d_cfr.bestAction(view, d_rng);
}

}
