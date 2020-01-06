#include "mcts_tree.hpp"

#include <cassert>

namespace jester {

MCTSTree::MCTSTree(const Game& game)
    : d_root(std::make_shared<MCTSNode>(game, nullptr))
{
}

std::shared_ptr<MCTSNode>& MCTSTree::select() {
    
}

void rollout(std::shared_ptr<MCTSNode>& node) {
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
    std::shared_ptr<MCTSNode> current(node);
    std::shared_ptr<MCTSNode> parent(current->parent().lock());
    while (parent != nullptr) {
        current->addReward(rewards[parent->currentPlayer()]);
        current->incrementPlayouts();
        current = parent;
        parent = current->parent().lock();
    }
    current->incrementPlayouts();
}

}
