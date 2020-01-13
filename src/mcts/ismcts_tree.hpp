#ifndef JESTER_ISMCTS_TREE_HPP
#define JESTER_ISMCTS_TREE_HPP

#include "mcts_node.hpp"

#include <mutex>

namespace jester {

class ISMCTSNode : public MCTSNode {
private:
    std::mutex d_mtx;

public:
    ISMCTSNode(size_t player);
    ~ISMCTSNode() = default;

    NodeExpansion expand(const Game& game);

    std::mutex& mutex();
};

class ISMCTSTree {
private:
    std::shared_ptr<ISMCTSNode> d_root;
    const GameView& d_view;
    Players d_players;
    std::mt19937 d_rng;

public:
    ISMCTSTree(const GameView& view);

    // Disable copy and assignment operators
    ISMCTSTree(const ISMCTSTree& tree) = delete;
    ISMCTSTree& operator=(const ISMCTSTree& tree) = delete;

    const std::shared_ptr<ISMCTSNode>& root() const;

    void iterate();

private:
    void selectPath(Game& game, std::vector<std::shared_ptr<ISMCTSNode>>& path);
    void rolloutPath(Game& game, const std::vector<std::shared_ptr<ISMCTSNode>>& path);
};

inline std::mutex& ISMCTSNode::mutex()
{
    return d_mtx;
}

inline const std::shared_ptr<ISMCTSNode>& ISMCTSTree::root() const
{
    return d_root;
}

}

#endif
