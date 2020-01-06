#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../rules/game.hpp"

#include <unordered_map>

namespace jester {

class MCTSNode {
private:
    size_t d_playouts;
    float d_reward;
    MCTSNode* d_parent_p;
    std::unordered_map<Action, MCTSNode*> d_children;
    std::vector<Action> d_unexpanded;
    size_t d_player;

public:
    MCTSNode(const Game& game, MCTSNode* parent);
    ~MCTSNode();

    size_t playouts() const;
    float reward() const;
    float rewardRatio() const;
    MCTSNode* parent() const;
    const std::unordered_map<Action, MCTSNode*>& children() const;
    size_t currentPlayer() const;
    bool fullyExpanded() const;
    std::ostream& print(std::ostream& os, size_t level = 0) const;

    MCTSNode* expand(Game& game);
    void addReward(float reward);
    void incrementPlayouts();
};

inline size_t MCTSNode::playouts() const
{
    return d_playouts;
}

inline float MCTSNode::reward() const
{
    return d_reward;
}

inline float MCTSNode::rewardRatio() const
{
    if (d_playouts == 0) {
        return 0;
    }
    return d_reward / d_playouts;
}

inline const std::unordered_map<Action, MCTSNode*>& MCTSNode::children() const
{
    return d_children;
}

inline MCTSNode* MCTSNode::parent() const
{
    return d_parent_p;
}

inline size_t MCTSNode::currentPlayer() const
{
    return d_player;
}

inline bool MCTSNode::fullyExpanded() const
{
    return d_unexpanded.empty();
}

inline void MCTSNode::addReward(float reward)
{
    d_reward += reward;
}

inline void MCTSNode::incrementPlayouts()
{
    d_playouts++;
}

}

#endif
