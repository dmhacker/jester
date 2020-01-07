#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../rules/game.hpp"
#include "mcts_stats.hpp"

#include <unordered_map>

namespace jester {

class MCTSNode {
private:
    MCTSStats d_stats;
    size_t d_player;
    MCTSNode* d_parent_p;
    std::unordered_map<Action, MCTSNode*> d_children;
    std::vector<Action> d_unexpanded;

public:
    MCTSNode(const Game& game, MCTSNode* parent);
    ~MCTSNode();

    // Disable copy and assignment operators
    MCTSNode(const MCTSNode& tree) = delete;
    MCTSNode& operator=(const MCTSNode& tree) = delete;

    bool fullyExpanded() const;
    size_t currentPlayer() const;
    MCTSNode* parent() const;

    std::unordered_map<Action, MCTSNode*>& children();
    MCTSStats& stats();
    Action expand();

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

inline bool MCTSNode::fullyExpanded() const
{
    return d_unexpanded.empty();
}

inline size_t MCTSNode::currentPlayer() const
{
    return d_player;
}

inline MCTSNode* MCTSNode::parent() const
{
    return d_parent_p;
}

inline std::unordered_map<Action, MCTSNode*>& MCTSNode::children()
{
    return d_children;
}

inline MCTSStats& MCTSNode::stats()
{
    return d_stats;
}

inline Action MCTSNode::expand()
{
    Action action = d_unexpanded.back();
    d_unexpanded.pop_back();
    return action;
}

}

#endif
