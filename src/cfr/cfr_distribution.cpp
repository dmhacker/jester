#include <cfr/cfr_distribution.hpp>

namespace jester {

CFRDistribution::CFRDistribution(size_t num_actions)
    : d_distribution(num_actions)
{
}

std::vector<float> CFRDistribution::regretMatching() const
{
    size_t num_actions = d_distribution.size();
    std::vector<float> profile(num_actions);
    float regret_sum = 0;
    for (size_t idx = 0; idx < num_actions; idx++) {
        float positive_regret = std::max(1.f * d_distribution[idx], 0.f);
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

std::vector<float> CFRDistribution::bestResponse() const
{
    size_t num_actions = d_distribution.size();
    std::vector<float> profile(num_actions);
    float profile_sum = 0;
    for (size_t idx = 0; idx < num_actions; idx++) {
        profile_sum += d_distribution[idx];
    }
    for (size_t idx = 0; idx < num_actions; idx++) {
        if (profile_sum > 0) {
            profile[idx] = d_distribution[idx] / profile_sum;
        } else {
            profile[idx] = 1.f / d_distribution.size();
        }
    }
    return profile;
}

}
