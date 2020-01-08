#include "dmcts_tree.hpp"

#include <cassert>
#include <cmath>

namespace jester {

DMCTSTree::DMCTSTree(const Game& game)
    : d_game(game)
    , d_root(new DMCTSNode(d_game, nullptr))
{
    assert(!d_game.finished());
}

DMCTSTree::~DMCTSTree()
{
    delete d_root;
}

DMCTSNode* DMCTSTree::selectAndExpand(Game& game)
{
    DMCTSNode* selection = d_root;
    std::shared_ptr<Action> next_action;
    while ((next_action = selection->unexpandedAction()) == nullptr) {
        if (game.finished()) {
            return selection;
        }
        Action best_action;
        DMCTSNode* best_node = nullptr;
        float best_score = -1;
        for (auto it : selection->children()) {
            auto child = it.second;
            float exploitation = child->stats().rewardRatio();
            float exploration = std::sqrt(
                2 * std::log(selection->stats().playouts())
                / child->stats().playouts());
            float score = exploitation + exploration;
            if (best_node == nullptr || score > best_score) {
                best_action = it.first;
                best_node = child;
                best_score = score;
            }
        }
        assert(best_node != nullptr);
        game.playAction(best_action);
        selection = best_node;
    }
    auto action = *next_action;
    game.playAction(action);
    auto child = new DMCTSNode(game, selection);
    selection->children()[action] = child;
    return child;
}

void DMCTSTree::rolloutAndPropogate(Game& game, DMCTSNode* node)
{
    game.play();
    auto& result = game.winOrder();
    std::unordered_map<size_t, float> rewards;
    for (size_t widx = 0; widx < result.size(); widx++) {
        size_t pid = result[widx];
        rewards[pid] = 1.f
            * (result.size() - 1 - widx)
            / (result.size() - 1);
    }
    DMCTSNode* current = node;
    DMCTSNode* parent = current->parent();
    while (parent != nullptr) {
        current->stats().addReward(rewards[parent->playerId()]);
        current->stats().incrementPlayouts();
        current = parent;
        parent = current->parent();
    }
    current->stats().incrementPlayouts();
}

}
