#ifndef JESTER_CFRM_STATS_HPP
#define JESTER_CFRM_STATS_HPP

#include "../rules/card.hpp"

#include <cereal/types/unordered_map.hpp>

namespace jester {

class CFRMStats {
private:
    std::unordered_map<Action, float> d_cumulProfile;
    std::unordered_map<Action, float> d_cumulRegret;

public:
    CFRMStats() = default;
    CFRMStats(const std::vector<Action>& actions);

    std::unordered_map<Action, float> currentProfile() const;
    std::unordered_map<Action, float> averageProfile() const;

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

}

#endif
