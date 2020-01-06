#ifndef JESTER_MCTS_TREE_HPP
#define JESTER_MCTS_TREE_HPP

#include "mcts_node.hpp"

namespace jester {

class MCTSTree {
private:
    std::shared_ptr<MCTSNode> d_root;

public:
    MCTSTree(const Game& game);
    void iterate(); 
private:
    std::shared_ptr<MCTSNode>& select();
    void rollout(std::shared_ptr<MCTSNode>& node);
};

inline void MCTSTree::iterate() {
    rollout(select());
}

}

#endif
