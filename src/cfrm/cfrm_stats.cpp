#include "cfrm_stats.hpp"
#include "../logging.hpp"

namespace jester {

CFRMStats::CFRMStats(size_t num_actions)
    : d_cumulProfile(num_actions)
    , d_cumulRegret(num_actions)
{
}

CFRMStats::CFRMStats(const CFRMStats& stats)
    : d_cumulProfile(stats.d_cumulProfile)
    , d_cumulRegret(stats.d_cumulRegret)
{
}

std::vector<float> CFRMStats::currentProfile() const
{
    size_t num_actions = d_cumulProfile.size();
    std::vector<float> profile(num_actions);
    float regret_sum = 0;
    for (size_t idx = 0; idx < num_actions; idx++) {
        float positive_regret = std::max(d_cumulRegret[idx], 0.f);
        profile[idx] = positive_regret;
        regret_sum += positive_regret;
    }
    for (size_t idx = 0; idx < num_actions; idx++) {
        if (regret_sum > 0) {
            profile[idx] /= regret_sum;
        } else {
            profile[idx] = 1.f / num_actions;
        }
    }
    return profile;
}

std::vector<float> CFRMStats::averageProfile() const
{
    size_t num_actions = d_cumulProfile.size();
    std::vector<float> profile(num_actions);
    float profile_sum = 0;
    for (size_t idx = 0; idx < num_actions; idx++) {
        profile_sum += d_cumulProfile[idx];
    }
    for (size_t idx = 0; idx < num_actions; idx++) {
        if (profile_sum > 0) {
            profile[idx] = d_cumulProfile[idx] / profile_sum;
        } else {
            profile[idx] = 1.f / d_cumulProfile.size();
        }
    }
    return profile;
}

std::ostream& operator<<(std::ostream& os, const CFRMStats& stats)
{
    return os << "{profile = " << stats.d_cumulProfile 
        << ", regret = " << stats.d_cumulRegret << "}";
}

}
