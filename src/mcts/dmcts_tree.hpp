#ifndef JESTER_DMCTS_TREE_HPP
#define JESTER_DMCTS_TREE_HPP

#include "dmcts_node.hpp"

namespace jester {

class DMCTSTree {
private:
    Game d_game;
    DMCTSNode* d_root;

public:
    DMCTSTree(const Game& game);
    ~DMCTSTree();

    // Disable copy and assignment operators
    DMCTSTree(const DMCTSTree& tree) = delete;
    DMCTSTree& operator=(const DMCTSTree& tree) = delete;

    DMCTSNode* root() const;

    void iterate();

private:
    DMCTSNode* selectAndExpand(Game& game);
    void rolloutAndPropogate(Game& game, DMCTSNode* node);
};

inline void DMCTSTree::iterate()
{
    Game game(d_game);
    rolloutAndPropogate(game, selectAndExpand(game));
}

inline DMCTSNode* DMCTSTree::root() const
{
    return d_root;
}

}

#endif
