#ifndef JESTER_CFRM_STATS_HPP
#define JESTER_CFRM_STATS_HPP

#include "../rules/card.hpp"

#include <mutex>

namespace jester {

class CFRMStats {
private:
    std::vector<float> d_cumulProfile;
    std::vector<float> d_cumulRegret;
    std::mutex d_mtx;

public:
    std::vector<Action> d_actions;

    CFRMStats() = default;
    CFRMStats(size_t num_actions);
    CFRMStats(const CFRMStats& stats);

    std::vector<float> currentProfile() const;
    std::vector<float> averageProfile() const;

    std::mutex& mutex();

    void addProfile(const std::vector<float>& profile);
    void addRegret(size_t idx, float regret);

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream&, const CFRMStats&);
};

inline void CFRMStats::addProfile(const std::vector<float>& profile)
{
    for (size_t i = 0; i < d_cumulProfile.size(); i++) {
        d_cumulProfile[i] += profile[i];
    }
}

inline void CFRMStats::addRegret(size_t idx, float regret)
{
    d_cumulRegret[idx] += regret;
}

template <class Archive>
inline void CFRMStats::serialize(Archive& archive)
{
    archive(d_cumulProfile, d_cumulRegret);
}

inline std::mutex& CFRMStats::mutex() {
    return d_mtx;
}

}

#endif
