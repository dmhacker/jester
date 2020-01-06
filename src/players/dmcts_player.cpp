#include "dmcts_player.hpp"
#include "../mcts/mcts_tree.hpp"
#include "../rules/game.hpp"
#include "random_player.hpp"

#include <iostream>
#include <mutex>
#include <thread>
#include <tuple>

namespace jester {

const std::chrono::milliseconds DMCTS_TIME_LIMIT = std::chrono::milliseconds(3000);

Action DMCTSPlayer::nextAction(const GameView& view)
{
    std::cerr << view.nextActions() << std::endl;
    std::unordered_map<Action, std::pair<int, int>> statistics;
    for (Action& action : view.nextActions()) {
        statistics[action] = std::make_pair<int, int>(0, 0);
    }
    if (statistics.size() == 1) {
        for (Action& action : view.nextActions()) {
            return action;
        }
    }
    std::mutex mtx;
    /* std::vector<std::thread> threads; */
    for (size_t i = 0; i < 3; i++) {
        /* threads.push_back(std::thread([&view, &time_limit, &mtx, &statistics]() { */
            std::vector<std::shared_ptr<Player>> players;
            for (size_t pid = 0; pid < view.playerCount(); pid++) {
                auto player = std::make_shared<RandomPlayer>();
                players.push_back(player);
            }
            Game game(players, view);
            MCTSTree tree(game);
            auto start_timestamp = std::chrono::system_clock::now();
            while (true) {
                auto end_timestamp = std::chrono::system_clock::now();
                if (end_timestamp - start_timestamp > DMCTS_TIME_LIMIT) {
                    break;
                }
                tree.iterate();
            }
            {
                std::lock_guard<std::mutex> lck(mtx);
                std::cerr << game;
                for (auto it : tree.root()->children()) {
                    statistics[it.first].first += it.second->reward();
                    statistics[it.first].second += it.second->playouts();
                    std::cerr << it.first << " " << it.second->reward() << " " << it.second->playouts() << std::endl;
                }
                /* tree.root()->print(std::cerr); */
            }
        /* })); */
    }
    /* for (auto& thr : threads) { */
    /*     thr.join(); */
    /* } */
    Action best_action;
    float best_ratio = -1;
    for (auto it : statistics) {
        float rat = it.second.first / it.second.second;
        if (rat > best_ratio) {
            best_action = it.first;
            best_ratio = rat;
        }
    }
    return best_action;
}

}
