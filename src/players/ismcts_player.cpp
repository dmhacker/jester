#include "ismcts_player.hpp"
#include "../mcts/ismcts_tree.hpp"

#include <iostream>
#include <thread>

namespace jester {

ISMCTSPlayer::ISMCTSPlayer(bool verbose, size_t workers,
    const std::chrono::milliseconds& time_limit)
    : d_verbose(verbose)
    , d_workerCount(workers)
    , d_timeLimit(time_limit)
{
}

Action ISMCTSPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }

    if (d_verbose) {
        std::cerr
            << "[P" << view.playerId()
            << "] ISMCTS has started." << std::endl;
    }

    // Build tree while we are within time limit
    ISMCTSTree tree(view);
    std::vector<std::thread> workers;
    const auto& limit = d_timeLimit;
    for (size_t widx = 0; widx < d_workerCount; widx++) {
        workers.push_back(std::thread([&tree, &limit]() {
            const auto start_timestamp = std::chrono::system_clock::now();
            while (true) {
                const auto end_timestamp = std::chrono::system_clock::now();
                if (end_timestamp - start_timestamp > limit) {
                    break;
                }
                tree.iterate();
            }
        }));
    }
    for (auto& thr : workers) {
        thr.join();
    }

    // Choose the best action (the one with the highest visits)
    Action best_action;
    size_t most_visits = 0;
    for (auto& action : actions) {
        auto& child = tree.root()->children()[action];
        auto& stats = child->stats();
        if (most_visits < stats.playouts()) {
            best_action = action;
            most_visits = stats.playouts();
        }
        if (d_verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] ISMCTS evaluated \"" << action
                << "\" as " << stats
                << "." << std::endl;
        }
    }

    return best_action;
}

}
