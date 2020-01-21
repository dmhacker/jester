#include <mcts/mcts_node.hpp>
#include <players/random_player.hpp>

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

}
