#include "ismcts_tree.hpp"
#include "../players/random_player.hpp"

#include <cassert>
#include <cmath>

namespace jester {

ISMCTSNode::ISMCTSNode(size_t player)
    : MCTSNode(player)
{
}

NodeExpansion ISMCTSNode::tryExpand(const Game& game)
{
    for (auto& action : game.nextActions()) {
        auto it = children().find(action);
        if (it == children().end()) {
            return NodeExpansion(action);
        }
    }
    return NodeExpansion();
}

ISMCTSTree::ISMCTSTree(const GameView& view)
    : d_root(new ISMCTSNode(view.playerId()))
    , d_view(view)
    , d_rng(std::random_device {}())

{
    for (size_t i = 0; i < d_view.playerCount(); i++) {
        d_players.push_back(std::make_shared<RandomPlayer>());
    }
    assert(!view.finished());
}

void ISMCTSTree::iterate()
{
    Game game(d_players, d_view, d_rng);
    std::vector<std::shared_ptr<ISMCTSNode>> path;
    selectPath(game, path);
    rolloutPath(game, path);
}

void ISMCTSTree::selectPath(Game& game, std::vector<std::shared_ptr<ISMCTSNode>>& path)
{

    auto selection = d_root;
    path.push_back(selection);
    NodeExpansion expansion;
    while (true) {
        std::lock_guard<std::mutex> plck(selection->mutex());
        expansion = selection->tryExpand(game);
        if (!expansion.empty()) {
            break;
        }
        if (game.finished()) {
            return;
        }
        Action best_action;
        std::shared_ptr<ISMCTSNode> best_node = nullptr;
        float best_score = -1;
        for (auto& action : game.nextActions()) {
            auto child = std::static_pointer_cast<ISMCTSNode>(
                selection->children()[action]);
            std::lock_guard<std::mutex> clck(child->mutex());
            float exploitation = child->stats().rewardRatio();
            float exploration = std::sqrt(
                2 * std::log(selection->stats().playouts())
                / child->stats().playouts());
            float score = exploitation + exploration;
            if (best_node == nullptr || score > best_score) {
                best_action = action;
                best_node = child;
                best_score = score;
            }
        }
        assert(best_node != nullptr);
        game.playAction(best_action);
        selection = best_node;
        path.push_back(selection);
    }
    auto& action = expansion.action();
    game.playAction(action);
    auto child = std::make_shared<ISMCTSNode>(
        game.currentPlayerId());
    path.push_back(child);
    {
        std::lock_guard<std::mutex> lck(selection->mutex());
        selection->children()[action] = std::move(child);
    }
}

void ISMCTSTree::rolloutPath(Game& game, const std::vector<std::shared_ptr<ISMCTSNode>>& path)
{
    game.play();
    auto& result = game.winOrder();
    std::unordered_map<size_t, float> rewards;
    for (size_t widx = 0; widx < result.size(); widx++) {
        size_t pid = result[widx];
        rewards[pid] = 1.f
            * (result.size() - 1 - widx)
            / (result.size() - 1);
    }
    {
        std::lock_guard<std::mutex> lck(d_root->mutex());
        d_root->stats().incrementPlayouts();
    }
    for (size_t i = 1; i < path.size(); i++) {
        auto parent = path[i - 1];
        auto current = path[i];
        std::lock_guard<std::mutex> lck(current->mutex());
        current->stats().addReward(rewards[parent->playerId()]);
        current->stats().incrementPlayouts();
    }
}

}
