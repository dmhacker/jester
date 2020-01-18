#ifndef JESTER_DMCTS_TREE_HPP
#define JESTER_DMCTS_TREE_HPP

#include "../game/game_state.hpp"
#include "mcts_node.hpp"

namespace jester {

class DMCTSNode : public MCTSNode {
private:
    std::vector<Action> d_unexpandedCache;
    bool d_cacheSetup;

public:
    DMCTSNode(size_t player);

    NodeExpansion expand(const GameState& game);
};

class DMCTSTree {
private:
    std::shared_ptr<DMCTSNode> d_root;
    GameState d_state;

public:
    DMCTSTree(const GameState& state);

    // Disable copy and assignment operators
    DMCTSTree(const DMCTSTree& tree) = delete;
    DMCTSTree& operator=(const DMCTSTree& tree) = delete;

    const std::shared_ptr<DMCTSNode>& root() const;

    void iterate();

private:
    void selectPath(GameState& state, std::vector<std::shared_ptr<DMCTSNode>>& path);
    void rolloutPath(GameState& state, const std::vector<std::shared_ptr<DMCTSNode>>& path);
};

inline const std::shared_ptr<DMCTSNode>& DMCTSTree::root() const
{
    return d_root;
}

}

#endif
