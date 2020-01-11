#include "tabular_cfrm.hpp"
#include "../rules/game.hpp"
#include "../rules/game_view.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>

namespace jester {

/* const static size_t CFRM_THREADS = std::max(1u, std::thread::hardware_concurrency()); */
const static size_t CFRM_THREADS = 1;

TabularCFRM::TabularCFRM()
{
    for (size_t i = 0; i < CFRM_THREADS; i++) {
        d_rngs.push_back(std::mt19937(std::random_device {}()));
    }
}

void TabularCFRM::iterate(bool verbose)
{
    std::vector<std::thread> threads;
    for (size_t t = 0; t < CFRM_THREADS; t++) {
        threads.push_back(std::thread([&]() {
            size_t num_players = 2;
            std::vector<std::shared_ptr<Player>> players(num_players);
            Game game(players);
            std::vector<float> reaches;
            for (size_t pid = 0; pid < num_players; pid++) {
                reaches.push_back(1.f);
            }
            for (size_t tpid = 0; tpid < num_players; tpid++) {
                train(verbose, tpid, game, reaches, d_rngs[0]);
            }
        }));
    }
    for (auto& thr : threads) {
        thr.join();
    }
}

Action TabularCFRM::bestAction(const GameView& view)
{
    CFRMAbstraction abstraction(view);
    auto it = d_stats.find(abstraction);
    if (it != d_stats.end()) {
        return sampleStrategy(it->second.averageStrategy(), d_rngs[0]);
    } else {
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        return actions[dist(d_rngs[0])];
    }
}

Action TabularCFRM::sampleStrategy(const std::unordered_map<Action, float>& strategy, std::mt19937& rng)
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    for (auto& it : strategy) {
        counter += it.second;
        if (randf <= counter) {
            return it.first;
        }
    }
    std::stringstream ss;
    ss << "Probability greater than 1: "
       << strategy;
    throw std::logic_error(ss.str());
}

float TabularCFRM::train(bool verbose, size_t tpid, const Game& game, const std::vector<float>& reaches, std::mt19937& rng)
{
    {
        std::lock_guard<std::mutex> lck(d_pmtx);
        std::cout << "\r"
                  << d_stats.size() << " information sets in storage."
                  << std::flush;
    }

    // Return reward/utility from terminal node; this is the evaluation function
    if (game.finished()) {
        for (size_t i = 0; i < game.playerCount(); i++) {
            auto pid = game.winOrder()[i];
            if (pid == tpid) {
                if (i < game.playerCount() - 1) {
                    return game.playerCount() - 1 - i;
                } else {
                    return -(game.playerCount() * (game.playerCount() - 1)) / 2;
                }
            }
        }
        return 0;
    }

    auto player = game.currentPlayerId();
    auto& actions = game.nextActions();
    auto view = game.currentPlayerView();
    if (actions.size() == 1) {
        auto action = actions[0];
        Game next_game(game);
        next_game.playAction(action);
        return train(verbose, tpid, next_game, reaches, rng);
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
    }
    auto& stats = stats_it->second;
    auto strategy = stats.strategy(reaches[player]);

    std::unordered_set<Action> a1;
    for (auto& action : game.nextActions()) {
        a1.insert(action);
    }
    std::unordered_set<Action> a2;
    for (auto& it : strategy) {
        a2.insert(it.first);
    }
    if (a1 != a2) {
        std::cerr << std::endl;
        std::cerr << game;
        std::cerr << abstraction;
        std::cerr << a1 << std::endl;
        std::cerr << a2 << std::endl;
    }
    assert(a1 == a2);

    if (player == tpid) {
        std::unordered_map<Action, float> child_util;
        float total_util = 0.0f;
        for (auto& action : actions) {
            Game next_game(game);
            next_game.playAction(action);
            std::vector<float> next_reaches(reaches);
            next_reaches[player] *= strategy[action];

            child_util[action] = train(verbose, tpid, next_game, next_reaches, rng);
            total_util += strategy[action] * child_util[action];
        }
        for (auto& action : game.nextActions()) {
            float regret = child_util[action] - total_util;
            stats.addRegret(action, reaches[player] * regret);
        }
        return total_util;
    } else {
        auto action = sampleStrategy(strategy, rng);
        Game next_game(game);
        next_game.playAction(action);
        std::vector<float> next_reaches(reaches);
        next_reaches[player] *= strategy[action];
        return train(verbose, tpid, next_game, next_reaches, rng);
    }
}

}
