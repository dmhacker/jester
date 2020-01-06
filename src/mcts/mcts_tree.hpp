#ifndef JESTER_MCTS_TREE_HPP
#define JESTER_MCTS_TREE_HPP

#include "mcts_node.hpp"

namespace jester {

class MCTSTree {
private:
    const Game& d_game;
    MCTSNode* d_root;

public:
    MCTSTree(const Game& game);
    ~MCTSTree();
    void iterate();

private:
    MCTSNode* select(Game& game);
    void rollout(Game& game, MCTSNode* node);
};

inline void MCTSTree::iterate()
{
    Game game(d_game);
    rollout(game, select(game));
}

}

#endif
