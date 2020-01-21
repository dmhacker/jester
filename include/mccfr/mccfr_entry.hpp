#ifndef JESTER_MCCFR_ENTRY_HPP
#define JESTER_MCCFR_ENTRY_HPP

#include "../game/card.hpp"

#include <mutex>

namespace jester {

class MCCFREntry {
private:
    std::vector<float> d_cumulProfile;
    std::vector<float> d_cumulRegret;

public:
    std::vector<Action> d_actions;

    MCCFREntry() = default;
    MCCFREntry(size_t num_actions);
    MCCFREntry(const MCCFREntry& entry);

    std::vector<float> currentProfile() const;
    std::vector<float> averageProfile() const;

    void addProfile(const std::vector<float>& profile);
    void addRegret(size_t idx, float regret);

    template <class Archive>
    void serialize(Archive& archive);
};

inline void MCCFREntry::addProfile(const std::vector<float>& profile)
{
    for (size_t i = 0; i < d_cumulProfile.size(); i++) {
        d_cumulProfile[i] += profile[i];
    }
}

inline void MCCFREntry::addRegret(size_t idx, float regret)
{
    d_cumulRegret[idx] += regret;
}

template <class Archive>
inline void MCCFREntry::serialize(Archive& archive)
{
    archive(d_cumulProfile, d_cumulRegret);
}

}

#endif
