#ifndef JESTER_DMCTS_NODE_HPP
#define JESTER_DMCTS_NODE_HPP

#include "mcts_stats.hpp"
#include "../rules/game.hpp"

#include <unordered_map>

namespace jester {

class DMCTSNode {
private:
    MCTSStats d_stats;
    size_t d_player;
    DMCTSNode* d_parent_p;
    std::unordered_map<Action, DMCTSNode*> d_children;
    std::vector<Action> d_unexpanded;

public:
    DMCTSNode(const Game& game, DMCTSNode* parent);
    ~DMCTSNode();

    // Disable copy and assignment operators
    DMCTSNode(const DMCTSNode& tree) = delete;
    DMCTSNode& operator=(const DMCTSNode& tree) = delete;

    bool fullyExpanded() const;
    size_t currentPlayer() const;
    DMCTSNode* parent() const;

    std::unordered_map<Action, DMCTSNode*>& children();
    MCTSStats& stats();
    Action expand();

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

inline bool DMCTSNode::fullyExpanded() const
{
    return d_unexpanded.empty();
}

inline size_t DMCTSNode::currentPlayer() const
{
    return d_player;
}

inline DMCTSNode* DMCTSNode::parent() const
{
    return d_parent_p;
}

inline std::unordered_map<Action, DMCTSNode*>& DMCTSNode::children()
{
    return d_children;
}

inline MCTSStats& DMCTSNode::stats()
{
    return d_stats;
}

inline Action DMCTSNode::expand()
{
    Action action = d_unexpanded.back();
    d_unexpanded.pop_back();
    return action;
}

}

#endif
