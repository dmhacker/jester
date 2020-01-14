#include "dmcts_player.hpp"
#include "../mcts/dmcts_tree.hpp"
#include "../rules/game_state.hpp"

#include <iostream>
#include <mutex>
#include <thread>

namespace jester {

DMCTSPlayer::DMCTSPlayer(bool verbose,
    size_t determinizations, size_t workers,
    const std::chrono::milliseconds& time_limit)
    : d_verbose(verbose)
    , d_determinizationCount(determinizations)
    , d_workerCount(workers)
    , d_timeLimit(time_limit)
{
}

Action DMCTSPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto& actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }

    if (d_verbose) {
        std::cerr
            << "[P" << view.playerId()
            << "] DMCTS has started." << std::endl;
    }

    // Create separate trees for possible determinizations
    std::vector<std::unique_ptr<DMCTSTree>> trees;
    std::deque<size_t> available;
    for (size_t i = 0; i < d_determinizationCount; i++) {
        std::mt19937 rng(std::random_device {}());
        GameState state(view, rng);
        trees.push_back(std::unique_ptr<DMCTSTree>(new DMCTSTree(state)));
        available.push_back(i);
    }

    // Workers repeatedly pick, build, and put back MCTS trees
    std::mutex mtx;
    std::vector<std::thread> workers;
    const auto& limit = d_timeLimit;
    for (size_t widx = 0; widx < d_workerCount; widx++) {
        workers.push_back(std::thread([&mtx, &trees, &available, &limit]() {
            const auto start_timestamp = std::chrono::system_clock::now();
            ssize_t choice = -1;
            while (true) {
                {
                    std::lock_guard<std::mutex> lck(mtx);
                    if (choice >= 0) {
                        available.push_back(choice);
                    }
                    choice = available.front();
                    available.pop_front();
                }
                const auto end_timestamp = std::chrono::system_clock::now();
                if (end_timestamp - start_timestamp > limit) {
                    break;
                }
                trees[choice]->iterate();
            }
        }));
    }
    for (auto& thr : workers) {
        thr.join();
    }

    // Choose the best action (the one with the highest visits)
    Action best_action;
    size_t most_visits = 0;
    for (auto action : actions) {
        MCTSStats stats;
        for (auto& tree : trees) {
            auto& children = tree->root()->children();
            auto it = children.find(action);
            if (it != children.end()) {
                stats.addStats(it->second->stats());
            }
        }
        if (most_visits < stats.playouts()) {
            best_action = action;
            most_visits = stats.playouts();
        }
        if (d_verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] DMCTS evaluated \"" << action
                << "\" as " << stats
                << "." << std::endl;
        }
    }

    return best_action;
}

}
