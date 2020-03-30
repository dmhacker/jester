#ifndef JESTER_CFR_DISTRIBUTION_HPP
#define JESTER_CFR_DISTRIBUTION_HPP

#include <cereal/types/vector.hpp>

namespace jester {

class CFRDistribution {
private:
    std::vector<int16_t> d_distribution;

public:
    CFRDistribution() = default;
    CFRDistribution(size_t num_actions);
    CFRDistribution(const CFRDistribution& entry) = default;

    std::vector<float> regretMatching() const;
    std::vector<float> bestResponse() const;

    void add(size_t idx, int16_t amount);

    template <class Archive>
    void serialize(Archive& archive);
};

inline void CFRDistribution::add(size_t idx, int16_t amount)
{
    d_distribution[idx] += amount;
}

template <class Archive>
inline void CFRDistribution::serialize(Archive& archive)
{
    archive(d_distribution);
}

}

#endif
