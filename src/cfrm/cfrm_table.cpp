#include "cfrm_table.hpp"
#include "../rules/game_state.hpp"
#include "../rules/game_view.hpp"
#include "../printing.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace jester {

CFRMTable::CFRMTable()
    : d_rng(std::random_device {}())
{
}

Action CFRMTable::bestAction(const GameView& view, bool verbose)
{
    CFRMKey key(view);
    auto it = d_table.find(key);
    if (it != d_table.end()) {
        auto actions = view.nextActions();
        std::sort(actions.begin(), actions.end());
        auto profile = it->second.averageProfile();
        if (verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] CFRM found distribution " << profile
                << " for actions " << actions
                << "." << std::endl;
        }
        return sample(actions, profile, d_rng);
    } else {
        if (verbose) {
            std::cerr
                << "[P" << view.playerId()
                << "] CFRM will choose a random action."
                << std::endl;
        }
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        return actions[dist(d_rng)];
    }
}

Action CFRMTable::sample(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng)
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    for (size_t idx = 0; idx < actions.size(); idx++) {
        counter += profile[idx];
        if (randf <= counter) {
            return actions[idx];
        }
    }
    std::stringstream ss;
    ss << "Distribution " << profile << " does not sum to 1.";
    throw std::logic_error(ss.str());
}

float CFRMTable::train(size_t tpid, const GameState& state, std::mt19937& rng)
{
    // Return utility from terminal node; this is the evaluation function
    if (state.finished()) {
        float reward = 0;
        for (size_t i = 0; i < state.playerCount(); i++) {
            auto pid = state.winOrder()[i];
            if (pid == tpid) {
                if (i < state.playerCount() - 1) {
                    reward = state.playerCount() - 1 - i;
                } else {
                    reward = state.playerCount() * (state.playerCount() - 1) / 2;
                    reward *= -1;
                }
                break;
            }
        }
        return reward;
    }

    // Skip over information sets where only one action is possible
    auto player = state.currentPlayerId();
    auto actions = state.nextActions();
    auto view = state.currentPlayerView();
    if (actions.size() == 1) {
        auto action = actions[0];
        GameState next_state(state);
        next_state.playAction(action);
        return train(tpid, next_state, rng);
    }
    std::sort(actions.begin(), actions.end());

    CFRMKey key(view);
    decltype(d_table)::iterator stats_it;
    {
        std::lock_guard<std::mutex> lck(d_mtx);
        stats_it = d_table.find(key);
        if (stats_it == d_table.end()) {
            auto new_stats = decltype(d_table)::value_type(
                key, CFRMStats(actions.size()));
            stats_it = d_table.insert(new_stats).first;
            stats_it->second.d_actions = actions;
        }
        std::cout << "\r"
                  << d_table.size() << " information sets in storage."
                  << std::flush;
    }
    auto& stats = stats_it->second;

    std::unique_lock<std::mutex> lck(stats.mutex());
    auto profile = stats.currentProfile();

    // External sampling: opponents only follow one action
    if (player != tpid) {
        auto action = sample(actions, profile, rng);
        GameState next_state(state);
        next_state.playAction(action);
        lck.unlock();
        float result = train(tpid, next_state, rng);
        lck.lock();
        return result;
    }

    // External sampling: walk every action for the current player
    lck.unlock();
    std::vector<float> child_util(actions.size());
    float total_util = 0.0f;
    for (size_t i = 0; i < actions.size(); i++) {
        GameState next_state(state);
        next_state.playAction(actions[i]);
        child_util[i] = train(tpid, next_state, rng);
        total_util += profile[i] * child_util[i];
    }
    lck.lock();
    // Update cumulative counterfactual regrets based off of utilities
    for (size_t i = 0; i < actions.size(); i++) {
        float regret = child_util[i] - total_util;
        stats.addRegret(i, regret);
    }
    stats.addProfile(profile);
    return total_util;
}

}
