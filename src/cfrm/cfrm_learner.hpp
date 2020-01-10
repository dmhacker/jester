#ifndef JESTER_ISMCTS_TREE_HPP
#define JESTER_ISMCTS_TREE_HPP

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"
#include "cfrm_node.hpp"
#include "compressed_view.hpp"

#include <mutex>

namespace jester {

class CFRMLearner {
private:
    std::unordered_map<CompressedView, CFRMNode> d_nodes;
    std::mt19937 d_rng;

public:
    void train(size_t iterations);
    ActionMap bestStrategy(const GameView& view);

private:
    std::vector<float> cfrm(Game& game, const std::vector<float>& reaches);
};

}

#endif
