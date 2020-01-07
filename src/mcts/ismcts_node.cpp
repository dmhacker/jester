#include "ismcts_node.hpp"
#include "../players/random_player.hpp"

namespace jester {

ISMCTSNode::ISMCTSNode(size_t player, ISMCTSNode* parent)
    : d_parent_p(parent)
    , d_player(player)
{
}

ISMCTSNode::~ISMCTSNode()
{
    for (auto& it : d_children) {
        delete it.second;
    }
}

std::shared_ptr<Action> ISMCTSNode::unexpandedAction(const Game& game) const
{
    for (auto& action : game.nextActions()) {
        auto it = d_children.find(action);
        if (it == d_children.end()) {
            return std::make_shared<Action>(action);
        }
    }
    return nullptr;
}

std::ostream& ISMCTSNode::print(std::ostream& os, size_t level) const
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
