#include "mcts_tree.hpp"

#include <cassert>
#include <cmath>

namespace jester {

MCTSTree::MCTSTree(const Game& game)
    : d_game(game)
    , d_root(new MCTSNode(d_game, nullptr))
{
    assert(!d_game.finished());
}

MCTSTree::~MCTSTree()
{
    delete d_root;
}

MCTSNode* MCTSTree::expandNode(Game& game, MCTSNode* node) const
{
    auto action = node->expand();
    game.playAction(action);
    auto child = new MCTSNode(game, node);
    node->children()[action] = child;
    return child;
}

MCTSNode* MCTSTree::selectAndExpand(Game& game)
{
    MCTSNode* selection = d_root;
    while (selection->fullyExpanded()) {
        if (game.finished()) {
            return selection;
        }
        Action best_action;
        MCTSNode* best_node = nullptr;
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
    return expandNode(game, selection);
}

void MCTSTree::rolloutAndPropogate(Game& game, MCTSNode* node)
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
    MCTSNode* current = node;
    MCTSNode* parent = current->parent();
    while (parent != nullptr) {
        current->stats().addReward(rewards[parent->currentPlayer()]);
        current->stats().incrementPlayouts();
        current = parent;
        parent = current->parent();
    }
    current->stats().incrementPlayouts();
}

}
