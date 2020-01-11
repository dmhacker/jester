#include "tabular_cfrm.hpp"
#include "../rules/game.hpp"
#include "../rules/game_view.hpp"

#include <cassert>
#include <iostream>

namespace jester {

TabularCFRM::TabularCFRM()
{
    randomizeSeed();
}

void TabularCFRM::randomizeSeed()
{
    d_rng = std::mt19937(std::random_device {}());
}

void TabularCFRM::iterate(bool verbose)
{
    for (size_t sz = 2; sz <= 6; sz++) {
        std::vector<std::shared_ptr<Player>> players(sz);
        Game game(players);
        std::vector<float> reaches;
        for (size_t pid = 0; pid < sz; pid++) {
            reaches.push_back(1.f);
        }
        for (size_t tpid = 0; tpid < sz; tpid++) {
            train(tpid, game, reaches);
        }
    }
}

template <class Archive>
void TabularCFRM::serialize(Archive& archive)
{
    archive(d_stats);
}

Action TabularCFRM::sampleStrategy(const std::unordered_map<Action, float>& strategy)
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(d_rng);
    float counter = 0.0f;
    for (auto& it : strategy) {
        counter += it.second;
        if (randf < counter) {
            return it.first;
        }
    }
    throw new std::bad_exception();
}

Action TabularCFRM::bestAction(const GameView& view)
{
    CFRMAbstraction abstraction(view);
    auto it = d_stats.find(abstraction);
    if (it != d_stats.end()) {
        return sampleStrategy(it->second.averageStrategy());
    } else {
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        return actions[dist(d_rng)];
    }
}

float TabularCFRM::train(bool verbose, size_t tpid, const Game& game, const std::vector<float>& reaches)
{
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

    CFRMAbstraction abstraction(view);
    auto stats_it = d_stats.find(abstraction);
    if (stats_it == d_stats.end()) {
        auto new_stats = decltype(d_stats)::value_type(
                                  abstraction, CFRMStats(game.nextActions()));
        stats_it = d_stats.insert(new_stats).first;
    }
    auto& stats = stats_it->second;
    auto strategy = stats.strategy(reaches[player]);

    if (player == tpid) {
        std::unordered_map<Action, float> child_util;
        float total_util = 0.0f;
        for (auto& action : actions) {
            Game next_game(game);
            next_game.playAction(action);
            std::vector<float> next_reaches(reaches);
            next_reaches[player] *= strategy[action];

            child_util[action] = train(tpid, next_game, next_reaches);
            total_util += strategy[action] * child_util[action];
        }
        for (auto& action : game.nextActions()) {
            float regret = child_util[action] - total_util;
            stats.addRegret(action, reaches[player] * regret);
        }
        return total_util;
    } else {
        auto action = sampleStrategy(strategy);
        Game next_game(game);
        next_game.playAction(action);
        std::vector<float> next_reaches(reaches);
        next_reaches[player] *= strategy[action];
        return train(tpid, next_game, next_reaches);
    }
}

}
