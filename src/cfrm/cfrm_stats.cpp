#include "cfrm_stats.hpp"

namespace jester {

CFRMStats::CFRMStats(const std::vector<Action>& actions)
{
    for (auto& action : actions) {
        d_cumulProfile[action] = 0;
        d_cumulRegret[action] = 0;
    }
}

CFRMStats::CFRMStats(const CFRMStats& stats)
    : d_cumulProfile(stats.d_cumulProfile)
    , d_cumulRegret(stats.d_cumulRegret)
{
}

std::unordered_map<Action, float> CFRMStats::currentProfile() const
{
    std::unordered_map<Action, float> profile(d_cumulProfile.size());
    float regret_sum = 0;
    for (auto& it : d_cumulRegret) {
        float positive_regret = std::max(it.second, 0.f);
        profile[it.first] = positive_regret;
        regret_sum += positive_regret;
    }
    for (auto& it : profile) {
        if (regret_sum > 0) {
            it.second /= regret_sum;
        } else {
            it.second = 1.f / d_cumulProfile.size();
        }
    }
    return profile;
}

std::unordered_map<Action, float> CFRMStats::averageProfile() const
{
    std::unordered_map<Action, float> profile(d_cumulProfile.size());
    float profile_sum = 0;
    for (auto& it : d_cumulProfile) {
        profile_sum += it.second;
    }
    for (auto& it : d_cumulProfile)
        if (profile_sum > 0) {
            profile[it.first] = it.second / profile_sum;
        } else {
            profile[it.first] = 1.f / d_cumulProfile.size();
        }
    return profile;
}

std::ostream& operator<<(std::ostream& os, const CFRMStats& stats)
{
    return os
        << stats.d_cumulProfile << std::endl
        << stats.d_cumulRegret << std::endl;
}

}
