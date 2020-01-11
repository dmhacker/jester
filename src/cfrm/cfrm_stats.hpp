#ifndef JESTER_CFRM_STATS_HPP
#define JESTER_CFRM_STATS_HPP

#include "../rules/card.hpp"

#include <unordered_map>

namespace jester {

class CFRMStats {
private:
    std::unordered_map<Action, float> d_strategy;
    std::unordered_map<Action, float> d_strategySum;
    std::unordered_map<Action, float> d_regretSum;

public:
    CFRMStats(const std::vector<Action>& actions);

    const std::unordered_map<Action, float>& strategy(float weight);
    std::unordered_map<Action, float> averageStrategy() const;
    void addRegret(const Action& action, float regret);

    template <class Archive>
    void serialize(Archive& archive);
};

}

#endif
