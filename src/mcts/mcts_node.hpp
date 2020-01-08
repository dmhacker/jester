#ifndef JESTER_MCTS_NODE_HPP
#define JESTER_MCTS_NODE_HPP

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"
#include "mcts_stats.hpp"

#include <unordered_map>

namespace jester {

// make type erased deleter
template <typename T>
std::function<void(void*)> makeErasedDeleter()
{
    return {
        [](void* p) {
            delete static_cast<T*>(p);
        }
    };
};

// A unique_ptr typedef
template <typename T>
using ErasedPtr = std::unique_ptr<T, std::function<void(void*)>>;

class MCTSNode {
private:
    MCTSStats d_stats;
    std::unordered_map<Action, ErasedPtr<MCTSNode>> d_children;
    size_t d_player;

public:
    MCTSNode(size_t player);
    virtual ~MCTSNode() = default;

    // Disable copy and assignment operators
    MCTSNode(const MCTSNode& tree) = delete;
    MCTSNode& operator=(const MCTSNode& tree) = delete;

    size_t playerId() const;

    std::unordered_map<Action, ErasedPtr<MCTSNode>>& children();
    MCTSStats& stats();

    virtual std::shared_ptr<Action> unexpandedAction(const Game& game) = 0;

    std::ostream& print(std::ostream& os, size_t level = 0) const;
};

inline size_t MCTSNode::playerId() const
{
    return d_player;
}

inline std::unordered_map<Action, ErasedPtr<MCTSNode>>& MCTSNode::children()
{
    return d_children;
}

inline MCTSStats& MCTSNode::stats()
{
    return d_stats;
}

}

#endif
