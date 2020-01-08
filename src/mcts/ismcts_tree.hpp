#ifndef JESTER_ISMCTS_TREE_HPP
#define JESTER_ISMCTS_TREE_HPP

#include "mcts_node.hpp"

namespace jester {

class ISMCTSNode : public MCTSNode {
public:
    ISMCTSNode(size_t player);
    std::shared_ptr<Action> unexpandedAction(const Game& game);
};

class ISMCTSTree {
private:
    const GameView& d_view;
    std::shared_ptr<ISMCTSNode> d_root;
    std::vector<std::shared_ptr<Player>> d_players;
    std::mt19937 d_rng;

public:
    ISMCTSTree(const GameView& view);

    // Disable copy and assignment operators
    ISMCTSTree(const ISMCTSTree& tree) = delete;
    ISMCTSTree& operator=(const ISMCTSTree& tree) = delete;

    const std::shared_ptr<ISMCTSNode>& root() const;

    void play();

private:
    void selectPath(Game& game, std::vector<std::shared_ptr<ISMCTSNode>>& path);
    void rolloutPath(Game& game, const std::vector<std::shared_ptr<ISMCTSNode>>& path);
};

inline const std::shared_ptr<ISMCTSNode>& ISMCTSTree::root() const
{
    return d_root;
}

}

#endif
