#ifndef JESTER_CFRM_NODE_HPP
#define JESTER_CFRM_NODE_HPP

#include "../rules/card.hpp"

#include <unordered_map>

namespace jester {

using ActionMap = std::unordered_map<Action, float>;

class CFRMNode {
private:
    ActionMap d_strategy;
    ActionMap d_strategySum;
    ActionMap d_regretSum;

public:
    CFRMNode(const std::vector<Action>& actions);

    const ActionMap& strategy(float weight);
    ActionMap averageStrategy() const;
    void addRegret(const Action& action, float regret);
};

}

#endif
