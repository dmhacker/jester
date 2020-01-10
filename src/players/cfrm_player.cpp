#include "cfrm_player.hpp"

#include <iostream>
#include <memory>
#include <thread>

namespace jester {

CFRMPlayer::CFRMPlayer(size_t iterations)
    : d_rng(std::random_device {}())
{
    d_learner.train(iterations, d_rng);
}

Action CFRMPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }

    auto strategy = d_learner.bestStrategy(view);
    std::cerr << strategy << std::endl;

    return d_learner.sampleStrategy(strategy, d_rng);
}

}
