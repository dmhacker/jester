#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../rules/game.hpp"

#include <unordered_map>

namespace jester {

class MCTSNode {
private:
    Game d_game;
    size_t d_playouts;
    float d_reward;
    std::weak_ptr<MCTSNode> d_parent;
    std::unordered_map<Action, std::shared_ptr<MCTSNode>> d_children;

public:
    MCTSNode(const Game& game, const std::shared_ptr<MCTSNode>& parent);

    const Game& game() const;
    size_t playouts() const;
    float reward() const;
    float rewardRatio() const;
    const std::unordered_map<Action, std::shared_ptr<MCTSNode>>& children() const;
    const std::weak_ptr<MCTSNode>& parent() const;
    bool terminal() const;
    size_t currentPlayer() const;

    void addReward(float reward);
};

inline const Game& MCTSNode::game() const
{
    return d_game;
}

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

inline const std::unordered_map<Action, std::shared_ptr<MCTSNode>>& MCTSNode::children() const
{
    return d_children;
}

inline const std::weak_ptr<MCTSNode>& MCTSNode::parent() const
{
    return d_parent;
}

inline bool MCTSNode::terminal() const
{
    return d_game.finished();
}

inline size_t MCTSNode::currentPlayer() const {
    if (d_game.currentAttack().size() == d_game.currentDefense().size()) {
        return d_game.attackerId();
    }
    else {
        return d_game.defenderId();
    }
}

inline void MCTSNode::addReward(float reward)
{
    d_reward += reward;
    d_playouts++;
}

}

#endif
