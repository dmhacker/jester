#ifndef JESTER_ISMCTS_NODE_HPP
#define JESTER_ISMCTS_NODE_HPP

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"
#include "mcts_stats.hpp"

#include <unordered_map>

namespace jester {

class ISMCTSNode {
private:
    MCTSStats d_stats;
    ISMCTSNode* d_parent_p;
    std::unordered_map<Action, ISMCTSNode*> d_children;
    size_t d_player;

public:
    ISMCTSNode(size_t player, ISMCTSNode* parent);
    ~ISMCTSNode();

    // Disable copy and assignment operators
    ISMCTSNode(const ISMCTSNode& tree) = delete;
    ISMCTSNode& operator=(const ISMCTSNode& tree) = delete;

    size_t playerId() const;
    ISMCTSNode* parent() const;

    std::unordered_map<Action, ISMCTSNode*>& children();
    MCTSStats& stats();
    std::shared_ptr<Action> unexpandedAction(const Game& game) const;

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

inline size_t ISMCTSNode::playerId() const
{
    return d_player;
}

inline ISMCTSNode* ISMCTSNode::parent() const
{
    return d_parent_p;
}

inline std::unordered_map<Action, ISMCTSNode*>& ISMCTSNode::children()
{
    return d_children;
}

inline MCTSStats& ISMCTSNode::stats()
{
    return d_stats;
}

}

#endif
