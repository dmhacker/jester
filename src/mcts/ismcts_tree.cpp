#include "ismcts_tree.hpp"
#include "../players/random_player.hpp"
#include "../game/game_state.hpp"

#include <cmath>

namespace jester {

ISMCTSNode::ISMCTSNode(size_t player)
    : MCTSNode(player)
{
}

NodeExpansion ISMCTSNode::expand(const GameState& state)
{
    for (auto& action : state.nextActions()) {
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
}

void ISMCTSTree::iterate()
{
    GameState state(d_view, d_rng);
    std::vector<std::shared_ptr<ISMCTSNode>> path;
    selectPath(state, path);
    rolloutPath(state, path);
}

void ISMCTSTree::selectPath(GameState& state, std::vector<std::shared_ptr<ISMCTSNode>>& path)
{

    auto selection = d_root;
    path.push_back(selection);
    NodeExpansion expansion;
    while (true) {
        std::lock_guard<std::mutex> plck(selection->mutex());
        expansion = selection->expand(state);
        if (!expansion.empty()) {
            break;
        }
        if (state.finished()) {
            return;
        }
        Action best_action;
        std::shared_ptr<ISMCTSNode> best_node = nullptr;
        float best_score = -1;
        for (auto& action : state.nextActions()) {
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
        state.playAction(best_action);
        selection = best_node;
        path.push_back(selection);
    }
    auto& action = expansion.action();
    state.playAction(action);
    auto child = std::make_shared<ISMCTSNode>(state.currentPlayerId());
    path.push_back(child);
    {
        std::lock_guard<std::mutex> lck(selection->mutex());
        selection->children()[action] = std::move(child);
    }
}

void ISMCTSTree::rolloutPath(GameState& state, const std::vector<std::shared_ptr<ISMCTSNode>>& path)
{
    RandomPlayer player;
    while (!state.finished()) {
        state.playAction(player.nextAction(state.currentPlayerView()));
    }
    auto& result = state.winOrder();
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
