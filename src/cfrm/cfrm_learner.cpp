#include "cfrm_learner.hpp"
#include "../players/random_player.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace jester {

void CFRMLearner::train(size_t iterations)
{
    for (size_t i = 0; i < iterations; i++) {
        std::vector<std::shared_ptr<Player>> players;
        for (size_t pid = 0; pid < 2; pid++) {
            players.push_back(std::make_shared<RandomPlayer>());
        }
        Game game(players);
        std::vector<float> reaches;
        for (size_t pid = 0; pid < 2; pid++) {
            reaches.push_back(1.f);
        }
        cfrm(game, reaches);
    }
}

ActionMap CFRMLearner::bestStrategy(const GameView& view)
{
    CompressedView cview(view);
    auto it = d_nodes.find(cview);
    if (it != d_nodes.end()) {
        return it->second.averageStrategy();
    }
    else {
        std::cerr << "Could not find an information set." << std::endl;
        ActionMap uniform;
        for (auto& action : view.nextActions()) {
            uniform[action] = 1.f / view.nextActions().size();
        }
        return uniform;
    }
}

std::vector<float> CFRMLearner::cfrm(Game& game, const std::vector<float>& reaches)
{
    if (game.finished()) {
        std::vector<float> rewards(game.playerCount());
        for (size_t i = 0; i < game.playerCount(); i++) {
            auto pid = game.winOrder()[i];
            if (i < game.playerCount() - 1) {
                rewards[pid] = game.playerCount() - 1 - i;
            }
            else {
                rewards[pid] = -(game.playerCount() * (game.playerCount() - 1)) / 2;
            }
        }
        return rewards;
    }

    auto player = game.currentPlayerId();
    auto view = game.currentPlayerView(); 
    CompressedView cview(view);
    auto nit = d_nodes.find(cview);
    if (nit == d_nodes.end()) {
        nit = d_nodes.insert(
                std::unordered_map<CompressedView, CFRMNode>::value_type(
                    cview, CFRMNode(game.nextActions()))).first; 
    }
    auto& node = nit->second;
    
    auto strategy = node.strategy(reaches[player]);
    ActionMap util(strategy);
    std::vector<float> node_util(game.playerCount());
}

}
