#include "ismcts_tree.hpp"
#include "../players/random_player.hpp"

#include <cassert>
#include <cmath>

namespace jester {

ISMCTSTree::ISMCTSTree(const GameView& view)
    : d_view(view)
    , d_root(new ISMCTSNode(view.playerId(), nullptr))
    , d_rng(std::random_device {}())

{
    for (size_t i = 0; i < d_view.playerCount(); i++) {
        d_players.push_back(std::make_shared<RandomPlayer>());
    }
    assert(!view.finished());
}

ISMCTSTree::~ISMCTSTree()
{
    delete d_root;
}

void ISMCTSTree::iterate()
{
    Game game(d_players, d_view, d_rng);
    rolloutAndPropogate(game, selectAndExpand(game));
}

ISMCTSNode* ISMCTSTree::selectAndExpand(Game& game)
{
    ISMCTSNode* selection = d_root;
    std::shared_ptr<Action> next_action = nullptr;
    while ((next_action = selection->unexpandedAction(game)) == nullptr) {
        if (game.finished()) {
            return selection;
        }
        Action best_action;
        ISMCTSNode* best_node = nullptr;
        float best_score = -1;
        for (auto& action : game.nextActions()) {
            auto child = selection->children()[action];
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
    }
    auto action = *next_action;
    game.playAction(action);
    auto child = new ISMCTSNode(game.currentPlayerId(), selection);
    selection->children()[action] = child;
    return child;
}

void ISMCTSTree::rolloutAndPropogate(Game& game, ISMCTSNode* node)
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
    ISMCTSNode* current = node;
    ISMCTSNode* parent = current->parent();
    while (parent != nullptr) {
        current->stats().addReward(rewards[parent->playerId()]);
        current->stats().incrementPlayouts();
        current = parent;
        parent = current->parent();
    }
    current->stats().incrementPlayouts();
}

}
