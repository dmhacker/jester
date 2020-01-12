#include "tabular_cfrm.hpp"
#include "../rules/game.hpp"
#include "../rules/game_view.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>

namespace jester {

const static size_t CFRM_THREADS = std::max(1u, std::thread::hardware_concurrency());

TabularCFRM::TabularCFRM()
{
    for (size_t i = 0; i < CFRM_THREADS; i++) {
        d_rngs.push_back(std::mt19937(std::random_device {}()));
    }
}

void TabularCFRM::iterate()
{
    std::vector<std::thread> threads;
    for (size_t t = 0; t < CFRM_THREADS; t++) {
        threads.push_back(std::thread([&]() {
            for (size_t a = 0; a < 100; a++) {
                size_t num_players = 2;
                std::vector<std::shared_ptr<Player>> players(num_players);
                Game game(players);
                for (size_t tpid = 0; tpid < num_players; tpid++) {
                    train(tpid, game, d_rngs[0]);
                }
            }
        }));
    }
    for (auto& thr : threads) {
        thr.join();
    }
}

Action TabularCFRM::bestAction(const GameView& view, bool verbose)
{
    CFRMAbstraction abstraction(view);
    auto it = d_stats.find(abstraction);
    if (it != d_stats.end()) {
        if (verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] CFRM found profile " 
                << it->second.averageProfile()
                << "." << std::endl;
        }
        return sample(it->second.averageProfile(), d_rngs[0]);
    } else {
        if (verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] CFRM will choose a random action."
                << std::endl;
        }
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        return actions[dist(d_rngs[0])];
    }
}

Action TabularCFRM::sample(const std::unordered_map<Action, float>& profile, std::mt19937& rng)
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    Action chosen;
    for (auto& it : profile) {
        counter += it.second;
        chosen = it.first;
        if (randf <= counter) {
            break;
        }
    }
    return chosen;
}

float TabularCFRM::train(size_t tpid, const Game& game, std::mt19937& rng)
{
    // Return utility from terminal node; this is the evaluation function
    if (game.finished()) {
        float reward = 0;
        for (size_t i = 0; i < game.playerCount(); i++) {
            auto pid = game.winOrder()[i];
            if (pid == tpid) {
                if (i < game.playerCount() - 1) {
                    reward = game.playerCount() - 1 - i;
                } else {
                    reward = game.playerCount() * (game.playerCount() - 1) / 2;
                    reward *= -1;
                }
                break;
            }
        }
        return reward;
    }

    // Skip over information sets where only one action is possible
    auto player = game.currentPlayerId();
    auto& actions = game.nextActions();
    auto view = game.currentPlayerView();
    if (actions.size() == 1) {
        auto action = actions[0];
        Game next_game(game);
        next_game.playAction(action);
        return train(tpid, next_game, rng);
    }

    CFRMAbstraction abstraction(view);
    decltype(d_stats)::iterator stats_it;
    {
        std::lock_guard<std::mutex> lck(d_mtx);
        stats_it = d_stats.find(abstraction);
        if (stats_it == d_stats.end()) {
            auto new_stats = decltype(d_stats)::value_type(
                abstraction, CFRMStats(game.nextActions()));
            stats_it = d_stats.insert(new_stats).first;
        }
        std::cout << "\r"
                  << d_stats.size() << " information sets in storage."
                  << std::flush;
    }
    auto& stats = stats_it->second;

    std::unique_lock<std::mutex> lck(stats.mutex());
    auto profile = stats.currentProfile();

    // External sampling: opponents only follow one action
    if (player != tpid) {
        auto action = sample(profile, rng);
        Game next_game(game);
        next_game.playAction(action);
        lck.unlock();
        float result = train(tpid, next_game, rng);
        lck.lock();
        return result;
    }

    // External sampling: walk every action for the current player
    lck.unlock();
    std::unordered_map<Action, float> child_util;
    float total_util = 0.0f;
    for (auto& action : actions) {
        Game next_game(game);
        next_game.playAction(action);
        child_util[action] = train(tpid, next_game, rng);
        total_util += profile[action] * child_util[action];
    }
    lck.lock();
    // Update cumulative counterfactual regrets based off of utilities
    for (auto& action : game.nextActions()) {
        float regret = child_util[action] - total_util;
        stats.addRegret(action, regret);
    }
    stats.addProfile(profile);
    return total_util;
}

}
