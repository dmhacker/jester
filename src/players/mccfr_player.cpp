#include <logs/loggers.hpp>
#include <players/mccfr_player.hpp>

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

MCCFRPlayer::MCCFRPlayer()
    : d_rng(std::random_device {}())
{
}

Action MCCFRPlayer::nextAction(const GameView& view)
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
    return d_engine.strategy().bestAction(view, d_rng);
}

}
