#include "mcts_tree.hpp"

#include <cassert>

namespace jester {

MCTSTree::MCTSTree(const Game& game)
    : d_root(new MCTSNode(game, nullptr))
{
}

MCTSTree::~MCTSTree() {
    delete d_root;
}

MCTSNode* MCTSTree::select() {
   MCTSNode* expansion = d_root->expand(); 
   while (expansion == nullptr) {
        // TODO: Use UCT algorithm to select best child node
        MCTSNode* visited; 
        expansion = visited->expand();
   }
   return expansion;
}

void rollout(MCTSNode* node) {
    Game simulation(node->game());
    simulation.play();
    auto& result = simulation.winOrder();
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
