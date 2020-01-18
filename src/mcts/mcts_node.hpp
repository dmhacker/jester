#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../game/card.hpp"
#include "mcts_stats.hpp"

#include <unordered_map>

namespace jester {

class GameState;

class NodeExpansion {
private:
    bool d_empty;
    Action d_action;

public:
    NodeExpansion();
    NodeExpansion(const Action& action);

    bool empty() const;
    const Action& action() const;
};

class MCTSNode {
private:
    MCTSStats d_stats;
    std::unordered_map<Action, std::shared_ptr<MCTSNode>> d_children;
    size_t d_player;

public:
    MCTSNode(size_t player);
    virtual ~MCTSNode() = default;

    // Disable copy and assignment operators
    MCTSNode(const MCTSNode& tree) = delete;
    MCTSNode& operator=(const MCTSNode& tree) = delete;

    size_t playerId() const;

    std::unordered_map<Action, std::shared_ptr<MCTSNode>>& children();
    MCTSStats& stats();

    virtual NodeExpansion expand(const GameState& state) = 0;
};

inline bool NodeExpansion::empty() const
{
    return d_empty;
}

inline const Action& NodeExpansion::action() const
{
    return d_action;
}

inline size_t MCTSNode::playerId() const
{
    return d_player;
}

inline std::unordered_map<Action, std::shared_ptr<MCTSNode>>& MCTSNode::children()
{
    return d_children;
}

inline MCTSStats& MCTSNode::stats()
{
    return d_stats;
}

}

#endif
