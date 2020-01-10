#ifndef JESTER_CFRM_LEARNER_HPP
#define JESTER_CFRM_LEARNER_HPP

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"
#include "cfrm_node.hpp"
#include "compressed_view.hpp"

#include <mutex>

namespace jester {

class CFRMLearner {
private:
    std::unordered_map<CompressedView, CFRMNode> d_nodes;
    std::unordered_map<CompressedView, Game> d_games;

public:
    void train(size_t iterations, std::mt19937& rng);
    ActionMap bestStrategy(const GameView& view);

    Action sampleStrategy(const ActionMap& strategy, std::mt19937& rng) const;
private:
    float cfrm(size_t tpid, const Game& game, const std::vector<float>& reaches, std::mt19937& rng);
};

}

#endif
