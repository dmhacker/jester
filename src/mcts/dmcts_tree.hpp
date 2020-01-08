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
    ~DMCTSNode() = default;

    std::shared_ptr<Action> unexpandedAction(const Game& game);
};

class DMCTSTree {
private:
    std::unique_ptr<DMCTSNode> d_root;
    Game d_game;

public:
    DMCTSTree(const Game& game);

    // Disable copy and assignment operators
    DMCTSTree(const DMCTSTree& tree) = delete;
    DMCTSTree& operator=(const DMCTSTree& tree) = delete;

    const std::unique_ptr<DMCTSNode>& root() const;

    void play();

private:
    void selectPath(Game& game, std::vector<DMCTSNode*>& path);
    void rolloutPath(Game& game, const std::vector<DMCTSNode*>& path);
};

inline const std::unique_ptr<DMCTSNode>& DMCTSTree::root() const
{
    return d_root;
}

}

#endif
