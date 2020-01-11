#ifndef JESTER_CFRM_STATS_HPP
#define JESTER_CFRM_STATS_HPP

#include "../rules/card.hpp"

#include <cereal/types/unordered_map.hpp>

namespace jester {

class CFRMStats {
private:
    std::unordered_map<Action, float> d_strategy;
    std::unordered_map<Action, float> d_strategySum;
    std::unordered_map<Action, float> d_regretSum;

public:
    CFRMStats() = default;
    CFRMStats(const std::vector<Action>& actions);

    const std::unordered_map<Action, float>& strategy(float weight);
    std::unordered_map<Action, float> averageStrategy() const;
    void addRegret(const Action& action, float regret);

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream&, const CFRMStats&);
};

template<class Archive>
inline void CFRMStats::serialize(Archive& archive)
{
    archive(d_strategy, d_strategySum, d_regretSum);
}

}

#endif
