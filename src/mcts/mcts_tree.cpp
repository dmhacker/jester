#include "mcts_tree.hpp"

#include <cmath>

namespace jester {

MCTSTree::MCTSTree(const Game& game)
    : d_game(game)
    , d_root(nullptr)
{
}

MCTSTree::~MCTSTree()
{
    delete d_root;
}

void MCTSTree::initialize()
{
    d_root = new MCTSNode(d_game, nullptr);
}

MCTSNode* MCTSTree::select(Game& game)
{
    MCTSNode* leaf = d_root;
    while (leaf->fullyExpanded()) {
        if (leaf->terminal()) {
            return leaf;
        }
        Action best_action;
        MCTSNode* best_node = nullptr;
        float best_score = -1;
        for (auto it : leaf->children()) {
            auto child = it.second;
            float exploitation = child->rewardRatio();
            float exploration = std::sqrt(2 * std::log(leaf->playouts()) / child->playouts());
            float score = exploitation + exploration;
            if (best_node == nullptr || score > best_score) {
                best_action = it.first;
                best_node = child;
                best_score = score;
            }
        }
        leaf = best_node;
    }
    return leaf->expand(game);
}

void MCTSTree::rollout(Game& game, MCTSNode* node)
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
        current->addReward(rewards[parent->currentPlayer()]);
        current->incrementPlayouts();
        current = parent;
        parent = current->parent();
    }
    current->incrementPlayouts();
}

}
