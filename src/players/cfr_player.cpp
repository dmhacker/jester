#include <players/cfr_player.hpp>
#include <players/ismcts_player.hpp>
#include <utils/logging.hpp>

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

CFRPlayer::CFRPlayer()
    : d_rng(std::random_device {}())
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
    ISMCTSPlayer test(1, std::chrono::milliseconds(2000));
    test.nextAction(view);
    return d_engine.strategy().bestAction(view, d_rng);
}

}
