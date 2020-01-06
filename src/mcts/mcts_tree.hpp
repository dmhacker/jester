#ifndef JESTER_MCTS_TREE_HPP
#define JESTER_MCTS_TREE_HPP

#include "mcts_node.hpp"

// TODO: Node does not store a game state, only maps action
// Only one game is created per iteration and passed through path we select

namespace jester {

class MCTSTree {
private:
    MCTSNode* d_root;

public:
    MCTSTree(const Game& game);
    ~MCTSTree();
    void iterate(); 
private:
    MCTSNode* select();
    void rollout(MCTSNode* node);
};

inline void MCTSTree::iterate() {
    rollout(select());
}

}

#endif
