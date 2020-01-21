#include <logs/loggers.hpp>
#include <mcts/ismcts_tree.hpp>
#include <players/ismcts_player.hpp>

#include <sstream>
#include <thread>

namespace jester {

ISMCTSPlayer::ISMCTSPlayer(size_t workers,
    const std::chrono::milliseconds& time_limit)
    : d_workerCount(workers)
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

    const auto start_mcts = std::chrono::system_clock::now();
    if (bots_logger != nullptr) {
        bots_logger->info("ISMCTS has been started for P{}.", view.playerId());
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
        if (bots_logger != nullptr) {
            std::stringstream ss;
            ss << "\"" << action << "\" as " << stats;
            bots_logger->info("ISMCTS evaluated {}.", ss.str());
        }
    }

    if (bots_logger != nullptr) {
        const auto end_mcts = std::chrono::system_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_mcts - start_mcts);
        bots_logger->info("ISMCTS finished evaluation in {} ms.",
            total_time.count());
    }

    return best_action;
}

}
