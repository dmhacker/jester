#include "dmcts_player.hpp"
#include "../mcts/dmcts_tree.hpp"
#include "../rules/game.hpp"
#include "random_player.hpp"

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

namespace jester {

const size_t DETERMINIZATION_COUNT = 6;
const size_t WORKER_COUNT = 4;
const std::chrono::milliseconds TIME_LIMIT = std::chrono::milliseconds(9000);

Action DMCTSPlayer::nextAction(const GameView& view)
{
    // If there is only one action then just take that action
    auto actions = view.nextActions();
    if (actions.size() == 1) {
        return actions[0];
    }

    // Create separate trees for possible determinizations
    std::vector<std::unique_ptr<DMCTSTree>> trees;
    std::deque<size_t> available;
    for (size_t i = 0; i < DETERMINIZATION_COUNT; i++) {
        std::vector<std::shared_ptr<Player>> players;
        for (size_t pid = 0; pid < view.playerCount(); pid++) {
            auto player = std::make_shared<RandomPlayer>();
            players.push_back(player);
        }
        std::mt19937 rng(std::random_device {}());
        Game game(players, view, rng);
        trees.push_back(std::unique_ptr<DMCTSTree>(new DMCTSTree(game)));
        available.push_back(i);
    }

    // Workers repeatedly pick, build, and put back MCTS trees
    std::mutex mtx;
    std::vector<std::thread> workers;
    for (size_t widx = 0; widx < WORKER_COUNT; widx++) {
        workers.push_back(std::thread([&mtx, &trees, &available]() {
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
                if (end_timestamp - start_timestamp > TIME_LIMIT) {
                    break;
                }
                trees[choice]->iterate();
            }
        }));
    }
    for (auto& thr : workers) {
        thr.join();
    }

    // Choose the best action (the one with the highest reward ratio)
    Action best_action;
    float best_ratio = -1;
    for (auto action : actions) {
        MCTSStats stats;
        for (auto& tree : trees) {
            auto& children = tree->root()->children();
            auto it = children.find(action);
            if (it != children.end()) {
                stats.addStats(it->second->stats());
            }
        }
        if (best_ratio < stats.rewardRatio()) {
            best_action = action;
            best_ratio = stats.rewardRatio();
        }
        std::cerr
            << "[P" << view.playerId()
            << "] DMCTS evaluated \"" << action
            << "\" as " << stats
            << "." << std::endl;
    }

    return best_action;
}

}
