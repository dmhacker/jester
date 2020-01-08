#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"
#include "mcts_stats.hpp"

#include <unordered_map>

namespace jester {

class MCTSNode {
private:
    MCTSStats d_stats;
    std::unordered_map<Action, std::shared_ptr<MCTSNode>> d_children;
    size_t d_player;

public:
    MCTSNode(size_t player);

    // Disable copy and assignment operators
    MCTSNode(const MCTSNode& tree) = delete;
    MCTSNode& operator=(const MCTSNode& tree) = delete;

    size_t playerId() const;

    std::unordered_map<Action, std::shared_ptr<MCTSNode>>& children();
    MCTSStats& stats();

    virtual std::shared_ptr<Action> unexpandedAction(const Game& game) = 0;

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

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
