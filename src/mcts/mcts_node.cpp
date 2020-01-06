#include "mcts_node.hpp"

namespace jester {

MCTSNode::MCTSNode(const Game& game, MCTSNode* parent)
    : d_playouts(0)
    , d_reward(0)
    , d_parent_p(parent)
    , d_unexpanded(game.nextActions())
{
    if (game.currentAttack().size() == game.currentDefense().size()) {
        d_player = game.attackerId();
    } else {
        d_player = game.defenderId();
    }
}

MCTSNode::~MCTSNode()
{
    for (auto& it : d_children) {
        delete it.second;
    }
}

MCTSNode* MCTSNode::expand(Game& game)
{
    auto action = d_unexpanded.back();
    d_unexpanded.pop_back();
    game.playAction(action);
    auto node = new MCTSNode(game, this);
    d_children[action] = node;
    return node;
}

std::ostream& MCTSNode::print(std::ostream& os, size_t level) const
{
    for (auto it : d_children) {
        for (size_t i = 0; i < level; i++) {
            os << "    ";
        }
        os << it.first 
            << " (pid = " << d_player 
            << ", rwd = " << d_reward 
            << ", cnt = " << d_playouts
            << ")" << std::endl;
        it.second->print(os, level + 1);
    }
    return os;
}

}
