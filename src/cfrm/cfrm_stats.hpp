#ifndef JESTER_CFRM_STATS_HPP
#define JESTER_CFRM_STATS_HPP

#include "../rules/card.hpp"

#include <cereal/types/unordered_map.hpp>
#include <mutex>

namespace jester {

class CFRMStats {
private:
    std::unordered_map<Action, float> d_cumulProfile;
    std::unordered_map<Action, float> d_cumulRegret;
    std::mutex d_mtx;

public:
    CFRMStats() = default;
    CFRMStats(const std::vector<Action>& actions);
    CFRMStats(const CFRMStats& stats);

    std::unordered_map<Action, float> currentProfile() const;
    std::unordered_map<Action, float> averageProfile() const;

    std::mutex& mutex();

    void addProfile(const std::unordered_map<Action, float>& profile);
    void addRegret(const Action& action, float regret);

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream&, const CFRMStats&);
};

inline void CFRMStats::addProfile(const std::unordered_map<Action, float>& profile)
{
    for (auto& it : profile) {
        d_cumulProfile[it.first] += it.second;
    }
}

inline void CFRMStats::addRegret(const Action& action, float regret)
{
    d_cumulRegret[action] += regret;
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
