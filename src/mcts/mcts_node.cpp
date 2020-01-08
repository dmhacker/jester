#include "mcts_node.hpp"
#include "../players/random_player.hpp"

namespace jester {

NodeExpansion::NodeExpansion()
    : d_empty(true)
{
}

NodeExpansion::NodeExpansion(const Action& action)
    : d_empty(false)
    , d_action(action)
{
}

MCTSNode::MCTSNode(size_t player)
    : d_player(player)
{
}

std::ostream& MCTSNode::print(std::ostream& os, size_t level) const
{
    if (level > 3) {
        return os;
    }
    for (auto& it : d_children) {
        for (size_t i = 0; i < level; i++) {
            os << "    ";
        }
        os << it.first
           << " (pid = " << d_player
           << ", stats = " << d_stats
           << ")" << std::endl;
        it.second->print(os, level + 1);
    }
    return os;
}

}
