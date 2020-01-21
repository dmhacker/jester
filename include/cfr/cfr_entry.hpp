#ifndef JESTER_CFR_ENTRY_HPP
#define JESTER_CFR_ENTRY_HPP

#include <cereal/types/vector.hpp>

namespace jester {

class CFREntry {
private:
    std::vector<int> d_cumulProfile;
    std::vector<int> d_cumulRegret;

public:
    CFREntry() = default;
    CFREntry(size_t num_actions);
    CFREntry(const CFREntry& entry);

    std::vector<float> currentProfile() const;
    std::vector<float> averageProfile() const;

    void addUtility(size_t idx, int utility);
    void addRegret(size_t idx, int regret);

    template <class Archive>
    void serialize(Archive& archive);
};

inline void CFREntry::addUtility(size_t idx, int utility)
{
    d_cumulProfile[idx] += utility;
}

inline void CFREntry::addRegret(size_t idx, int regret)
{
    d_cumulRegret[idx] += regret;
}

template <class Archive>
inline void CFREntry::serialize(Archive& archive)
{
    archive(d_cumulProfile, d_cumulRegret);
}

}

#endif
