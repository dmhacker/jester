#include "cfrm_learner.hpp"
#include "../players/random_player.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace jester {

void CFRMLearner::train(size_t iterations, std::mt19937& rng)
{
    for (size_t i = 0; i < iterations; i++) {
        for (size_t sz = 2; sz < 6; sz++) {
            std::vector<std::shared_ptr<Player>> players;
            for (size_t pid = 0; pid < sz; pid++) {
                players.push_back(std::make_shared<RandomPlayer>());
            }
            Game game(players);
            std::vector<float> reaches;
            for (size_t pid = 0; pid < sz; pid++) {
                reaches.push_back(1.f);
            }
            for (size_t tpid = 0; tpid < sz; tpid++) {
                cfrm(tpid, game, reaches, rng);
                std::cerr << "Done with one iteration." << std::endl;
            }
        }
    }
}

ActionMap CFRMLearner::bestStrategy(const GameView& view)
{
    CompressedView cview(view);
    auto it = d_nodes.find(cview);
    if (it != d_nodes.end()) {
        return it->second.averageStrategy();
    } else {
        std::cerr << "Could not find an information set." << std::endl;
        ActionMap uniform;
        for (auto& action : view.nextActions()) {
            uniform[action] = 1.f / view.nextActions().size();
        }
        return uniform;
    }
}

float CFRMLearner::cfrm(size_t tpid, const Game& game, const std::vector<float>& reaches, std::mt19937& rng)
{
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
    CompressedView cview(view);
    auto nit = d_nodes.find(cview);
    if (nit == d_nodes.end()) {
        nit = d_nodes.insert(
                         std::unordered_map<CompressedView, CFRMNode>::value_type(
                             cview, CFRMNode(game.nextActions())))
                  .first;
    }
    auto& node = nit->second;
    auto strategy = node.strategy(reaches[player]);

    std::unordered_set<Action> a1;
    for (auto& action : actions) {
        a1.insert(action);
    }
    std::unordered_set<Action> a2;
    for (auto& it : strategy) {
        a2.insert(it.first);
    }
    if (a1 != a2) {
        std::cerr << strategy << std::endl;
        std::cerr << actions << std::endl;
        std::cerr << game;
        std::cerr << cview; 
        std::cerr << std::endl;
    }

    if (player == tpid) {
        std::unordered_map<Action, float> child_util;
        float total_util = 0.0f;
        for (auto& action : actions) {
            Game next_game(game);
            next_game.playAction(action);
            std::vector<float> next_reaches(reaches);
            next_reaches[player] *= strategy[action];

            child_util[action] = cfrm(tpid, next_game, next_reaches, rng);
            total_util += strategy[action] * child_util[action];
        }
        for (auto& action : game.nextActions()) {
            float regret = child_util[action] - total_util;
            node.addRegret(action, reaches[player] * regret);
        }
        return total_util;
    } else {
        auto action = sampleStrategy(strategy, rng);
        Game next_game(game);
        next_game.playAction(action);
        std::vector<float> next_reaches(reaches);
        next_reaches[player] *= strategy[action];
        return cfrm(tpid, next_game, next_reaches, rng);
    }

    const float exploration = 0.6f;
    std::discrete_distribution<int> dist { exploration, 1 - exploration };
    Action chosen;
    if (dist(rng) == 0) {
        std::uniform_int_distribution<std::mt19937::result_type> dist2(0, actions.size() - 1);
        chosen = actions[dist2(rng)];
    } else {
        chosen = sampleStrategy(strategy, rng);
    }
}

Action CFRMLearner::sampleStrategy(const ActionMap& strategy, std::mt19937& rng) const
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    for (auto& it : strategy) {
        counter += it.second;
        if (randf < counter) {
            return it.first;
        }
    }
    throw new std::bad_exception();
}

}
