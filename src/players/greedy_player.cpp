#include "greedy_player.hpp"

namespace jester {

GreedyPlayer::GreedyPlayer()
    : d_rng(d_dev())
{
}

Action GreedyPlayer::attack(const GameView& view, std::chrono::milliseconds time_limit)
{
    auto actions = view.nextActions();
    std::vector<Action> filtered;
    for (auto& action : actions) {
        if (!action.empty()) {
            filtered.push_back(action);
        }
    }
    if (filtered.empty()) {
        return Action();
    }
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, filtered.size() - 1);
    return filtered[dist(d_rng)];

}

Action GreedyPlayer::defend(const GameView& view, std::chrono::milliseconds time_limit)
{
    auto actions = view.nextActions();
    std::vector<Action> filtered;
    for (auto& action : actions) {
        if (!action.empty()) {
            filtered.push_back(action);
        }
    }
    if (filtered.empty()) {
        return Action();
    }
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, filtered.size() - 1);
    return filtered[dist(d_rng)];
}

}
