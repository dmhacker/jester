#include "cfrm_stats.hpp"

namespace jester {

CFRMStats::CFRMStats(const std::vector<Action>& actions)
{
    for (auto& action : actions) {
        d_strategy[action] = 0;
        d_strategySum[action] = 0;
        d_regretSum[action] = 0;
    }
}

// Taken from modelai.gettysburg.edu/2013/cfr/cfr.pdfd for now
// FIXME: It will be replaced in the future.

const std::unordered_map<Action, float>& CFRMStats::strategy(float weight)
{
    float normal_sum = 0;
    for (const auto& it : d_regretSum) {
        float piece = it.second > 0 ? it.second : 0;
        d_strategy[it.first] = piece;
        normal_sum += piece;
    }
    for (auto& it : d_strategy) {
        if (normal_sum > 0)
            it.second /= normal_sum;
        else
            it.second = 1.f / d_strategy.size();
        d_strategySum[it.first] += weight * it.second;
    }
    return d_strategy;
}

std::unordered_map<Action, float> CFRMStats::averageStrategy() const
{
    std::unordered_map<Action, float> averages(d_strategy.size());
    float normal_sum = 0;
    for (const auto& it : d_strategySum)
        normal_sum += it.second;
    for (const auto& it : d_strategySum)
        if (normal_sum > 0)
            averages[it.first] = it.second / normal_sum;
        else
            averages[it.first] = 1.0 / d_strategy.size();
    return averages;
}

void CFRMStats::addRegret(const Action& action, float regret)
{
    d_regretSum[action] += regret;
}

}
