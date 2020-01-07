#ifndef JESTER_DMCTS_NODE_HPP
#define JESTER_DMCTS_NODE_HPP

#include "mcts_stats.hpp"
#include "../rules/game.hpp"

#include <unordered_map>

namespace jester {

class DMCTSNode {
private:
    MCTSStats d_stats;
    DMCTSNode* d_parent_p;
    std::unordered_map<Action, DMCTSNode*> d_children;
    size_t d_player;
    std::vector<Action> d_unexpanded;

public:
    DMCTSNode(const Game& game, DMCTSNode* parent);
    ~DMCTSNode();

    // Disable copy and assignment operators
    DMCTSNode(const DMCTSNode& tree) = delete;
    DMCTSNode& operator=(const DMCTSNode& tree) = delete;

    size_t playerId() const;
    DMCTSNode* parent() const;

    std::unordered_map<Action, DMCTSNode*>& children();
    MCTSStats& stats();
    std::shared_ptr<Action> unexpandedAction();

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

inline size_t DMCTSNode::playerId() const
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

}

#endif
