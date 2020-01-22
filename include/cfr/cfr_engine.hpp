#ifndef JESTER_CFR_ENGINE_HPP
#define JESTER_CFR_ENGINE_HPP

#include "../game/card.hpp"
#include "cfr_table.hpp"

#include <chrono>
#include <random>
#include <stda/erased_ptr.hpp>

namespace jester {

class GameState;
class GameView;

class CFREngine {
private:
    stda::erased_ptr<CFRTable> d_table;

public:
    CFREngine();
    CFREngine(stda::erased_ptr<CFRTable>&& table);

    Action bestAction(const GameView& view, std::mt19937& rng);
    size_t sampleIndex(const std::vector<float>& profile, std::mt19937& rng) const;

    void train();
    int train(size_t tpid, const GameState& state, std::mt19937& rng);

    CFRTable* table();
};

inline CFRTable* CFREngine::table()
{
    return d_table.get();
}

}

#endif
