#ifndef JESTER_ISMCTS_TREE_HPP
#define JESTER_ISMCTS_TREE_HPP

#include "ismcts_node.hpp"

namespace jester {

class ISMCTSTree {
private:
    const GameView& d_view;
    ISMCTSNode* d_root;
    std::vector<std::shared_ptr<Player>> d_players;
    std::mt19937 d_rng;

public:
    ISMCTSTree(const GameView& view);
    ~ISMCTSTree();

    // Disable copy and assignment operators
    ISMCTSTree(const ISMCTSTree& tree) = delete;
    ISMCTSTree& operator=(const ISMCTSTree& tree) = delete;

    ISMCTSNode* root() const;

    void iterate();

private:
    ISMCTSNode* selectAndExpand(Game& game);
    void rolloutAndPropogate(Game& game, ISMCTSNode* node);
};

inline ISMCTSNode* ISMCTSTree::root() const
{
    return d_root;
}

}

#endif
