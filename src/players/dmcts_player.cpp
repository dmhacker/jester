#include "dmcts_player.hpp"
#include "../mcts/mcts_tree.hpp"
#include "../rules/game.hpp"
#include "random_player.hpp"

#include <mutex>
#include <thread>
#include <iostream>
#include <sstream>

namespace jester {

const size_t DETERMINIZATION_COUNT = 3;
const std::chrono::milliseconds TIME_LIMIT = std::chrono::milliseconds(4000);

namespace {
    std::string playerPrefix(size_t pid) {
        std::stringstream ss;
        ss << "[P" << pid << "] [THINKING] ";
        return ss.str();
    }
}

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
    const auto start_timestamp = std::chrono::system_clock::now();
    for (size_t i = 0; i < DETERMINIZATION_COUNT; i++) {
        threads.push_back(std::thread([&view, &mtx, &statistics, &start_timestamp]() {
            std::vector<std::shared_ptr<Player>> players;
            for (size_t pid = 0; pid < view.playerCount(); pid++) {
                auto player = std::make_shared<RandomPlayer>();
                players.push_back(player);
            }
            // Create a determinization of the current game
            std::mt19937 rng(std::random_device{}());
            Game game(players, view, rng);
            MCTSTree tree(game);
            // While we have enough time, incrementally build the tree 
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
            << playerPrefix(view.playerId())
            << "\"" << it.first 
            << "\" has DMCTS stats " << it.second 
            << "." << std::endl;
    }
                
    // Print out actual time spent on calculating moves
    const auto end_timestamp = std::chrono::system_clock::now();
    std::cerr 
        << playerPrefix(view.playerId())
        << "Total time spent was "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end_timestamp - start_timestamp).count()
        << " ms." << std::endl;

    return best_action;
}

}
