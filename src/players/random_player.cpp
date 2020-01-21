#include <players/random_player.hpp>

namespace jester {

RandomPlayer::RandomPlayer()
    : d_rng(std::random_device{}())
{
}

Action RandomPlayer::nextAction(const GameView& view)
{
    auto actions = view.nextActions();
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
    return actions[dist(d_rng)];
}

}
