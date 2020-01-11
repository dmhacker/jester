#include "dmcts_tree.hpp"

#include <cassert>
#include <cmath>

namespace jester {

DMCTSNode::DMCTSNode(size_t player)
    : MCTSNode(player)
    , d_cacheSetup(false)
{
}

NodeExpansion DMCTSNode::tryExpand(const Game& game)
{
    if (game.finished()) {
        return NodeExpansion();
    }
    if (!d_cacheSetup) {
        d_unexpandedCache = game.nextActions();
        d_cacheSetup = true;
    }
    if (d_unexpandedCache.empty()) {
        return NodeExpansion();
    }
    NodeExpansion expansion(d_unexpandedCache.back());
    d_unexpandedCache.pop_back();
    return expansion;
}

DMCTSTree::DMCTSTree(const Game& game)
    : d_root(new DMCTSNode(game.currentPlayerId()))
    , d_game(game)
{
    assert(!game.finished());
}

void DMCTSTree::iterate()
{
    Game game(d_game);
    std::vector<std::shared_ptr<DMCTSNode>> path;
    selectPath(game, path);
    rolloutPath(game, path);
}

void DMCTSTree::selectPath(Game& game, std::vector<std::shared_ptr<DMCTSNode>>& path)
{
    auto selection = d_root;
    path.push_back(selection);
    NodeExpansion expansion;
    while ((expansion = selection->tryExpand(game)).empty()) {
        if (game.finished()) {
            return;
        }
        Action best_action;
        std::shared_ptr<DMCTSNode> best_node = nullptr;
        float best_score = -1;
        for (auto& it : selection->children()) {
            auto child = std::static_pointer_cast<DMCTSNode>(it.second);
            float exploitation = child->stats().rewardRatio();
            float exploration = std::sqrt(
                2 * std::log(selection->stats().playouts())
                / child->stats().playouts());
            float score = exploitation + exploration;
            if (best_node == nullptr || score > best_score) {
                best_action = it.first;
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
    auto child = std::make_shared<DMCTSNode>(game.currentPlayerId());
    path.push_back(child);
    selection->children()[action] = child;
}

void DMCTSTree::rolloutPath(Game& game, const std::vector<std::shared_ptr<DMCTSNode>>& path)
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
    d_root->stats().incrementPlayouts();
    for (size_t i = 1; i < path.size(); i++) {
        auto parent = path[i - 1];
        auto current = path[i];
        current->stats().addReward(rewards[parent->playerId()]);
        current->stats().incrementPlayouts();
    }
}

}
