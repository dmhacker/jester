#include "ismcts_node.hpp"
#include "../players/random_player.hpp"

namespace jester {

ISMCTSNode::ISMCTSNode(size_t player, ISMCTSNode* parent)
    : d_player(player)
    , d_parent_p(parent)
{
}

ISMCTSNode::~ISMCTSNode()
{
    for (auto& it : d_children) {
        delete it.second;
    }
}

inline std::shared_ptr<std::tuple<Game, Action>> ISMCTSNode::expand(const GameView& view, std::mt19937& rng)
{
    std::vector<std::shared_ptr<Player>> players;
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        players.push_back(std::make_shared<RandomPlayer>());
    }
    Game game(players, view, rng);
    for (auto& action : game.nextActions()) {
        auto it = d_children.find(action);
        if (it == d_children.end()) {
            return std::make_shared<std::tuple<Game, Action>>(std::make_pair<>(game, action)); 
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
