#include "dmcts_node.hpp"

namespace jester {

DMCTSNode::DMCTSNode(const Game& game, DMCTSNode* parent)
    : d_parent_p(parent)
    , d_unexpanded(game.nextActions())
{
    if (game.currentAttack().size() == game.currentDefense().size()) {
        d_player = game.attackerId();
    } else {
        d_player = game.defenderId();
    }
}

DMCTSNode::~DMCTSNode()
{
    for (auto& it : d_children) {
        delete it.second;
    }
}

std::ostream& DMCTSNode::print(std::ostream& os, size_t level) const
{
    if (level > 3) {
        return os;
    }
    for (auto it : d_children) {
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
