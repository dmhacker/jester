#ifndef JESTER_DMCTS_TREE_HPP
#define JESTER_DMCTS_TREE_HPP

#include "mcts_node.hpp"

namespace jester {

class DMCTSNode : public MCTSNode {
private:
    std::vector<Action> d_unexpandedCache;
    bool d_cacheSetup;

public:
    DMCTSNode(size_t player);
    std::shared_ptr<Action> unexpandedAction(const Game& game);
};

class DMCTSTree {
private:
    Game d_game;
    std::shared_ptr<DMCTSNode> d_root;

public:
    DMCTSTree(const Game& game);

    // Disable copy and assignment operators
    DMCTSTree(const DMCTSTree& tree) = delete;
    DMCTSTree& operator=(const DMCTSTree& tree) = delete;

    const std::shared_ptr<DMCTSNode>& root() const;

    void play();

private:
    void selectPath(Game& game, std::vector<std::shared_ptr<DMCTSNode>>& path);
    void rolloutPath(Game& game, const std::vector<std::shared_ptr<DMCTSNode>>& path);
};

inline const std::shared_ptr<DMCTSNode>& DMCTSTree::root() const
{
    return d_root;
}

}

#endif
