#include "dmcts_player.hpp"
#include "../mcts/mcts_tree.hpp"
#include "../rules/game.hpp"
#include "random_player.hpp"

#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

namespace jester {

const size_t DETERMINIZATION_COUNT = 4;
const std::chrono::milliseconds TIME_LIMIT = std::chrono::milliseconds(4000);

Action DMCTSPlayer::nextAction(const GameView& view)
{
    // Set up statistics for each action
    std::unordered_map<Action, MCTSStats> statistics;
    for (auto& action : view.nextActions()) {
        statistics[action] = MCTSStats();
    }

    // If there is only one action then just take that action
    if (statistics.size() == 1) {
        for (auto& it : statistics) {
            return it.first;
        }
    }

    // Run N simulations on N threads for the given time limit
    std::mutex mtx;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < DETERMINIZATION_COUNT; i++) {
        threads.push_back(std::thread([&view, &mtx, &statistics]() {
            std::vector<std::shared_ptr<Player>> players;
            for (size_t pid = 0; pid < view.playerCount(); pid++) {
                auto player = std::make_shared<RandomPlayer>();
                players.push_back(player);
            }
            // Create a determinization of the current game
            std::mt19937 rng(std::random_device {}());
            Game game(players, view, rng);
            MCTSTree tree(game);
            // While we have enough time, incrementally build the tree
            const auto start_timestamp = std::chrono::system_clock::now();
            while (true) {
                const auto end_timestamp = std::chrono::system_clock::now();
                if (end_timestamp - start_timestamp > TIME_LIMIT) {
                    break;
                }
                tree.iterate();
            }
            // Accrue stats once the tree has been developed enough
            {
                std::lock_guard<std::mutex> lck(mtx);
                for (auto it : tree.root()->children()) {
                    auto& master_stats = statistics[it.first];
                    auto& stats = it.second->stats();
                    master_stats.addStats(stats);
                }
            }
        }));
    }
    for (auto& thr : threads) {
        thr.join();
    }

    // Choose the best action (the one with the highest reward ratio)
    Action best_action;
    float best_ratio = -1;
    for (auto it : statistics) {
        float ratio = it.second.rewardRatio();
        if (ratio > best_ratio) {
            best_action = it.first;
            best_ratio = ratio;
        }
        std::cerr
            << "[P" << view.playerId()
            << "] \"" << it.first
            << "\" has DMCTS stats " << it.second
            << "." << std::endl;
    }

    return best_action;
}

}
