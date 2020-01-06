#include "mcts_node.hpp"

namespace jester {

MCTSNode::MCTSNode(const Game& game, MCTSNode* parent)
    : d_game(game)
    , d_playouts(0)
    , d_reward(0)
    , d_parent_p(parent)
    , d_unexpanded(game.nextActions())
{
}
    
MCTSNode::~MCTSNode() {
    for (auto& it : d_children) {
        delete it.second;
    }
}

MCTSNode* MCTSNode::expand()
{
    if (d_unexpanded.empty()) {
        return nullptr;
    }
    auto action = d_unexpanded.back();
    d_unexpanded.pop_back();
    Game game(d_game);
    game.playAction(action);
    auto node = new MCTSNode(game, this);
    d_children[action] = node;
    return node;
}

}
