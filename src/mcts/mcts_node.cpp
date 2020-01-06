#include "mcts_node.hpp"

namespace jester {

MCTSNode::MCTSNode(const Game& game, const std::shared_ptr<MCTSNode>& parent)
    : d_game(game)
    , d_playouts(0)
    , d_reward(0)
    , d_parent(parent)
{
}

}
