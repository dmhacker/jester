#ifndef JESTER_MCCFR_INFOSET_HPP
#define JESTER_MCCFR_INFOSET_HPP

#include <memory>
#include <cereal/types/vector.hpp>

namespace jester {

class MCCFRInfoSet;

}

namespace std {

template <>
struct hash<jester::MCCFRInfoSet>;

}

namespace jester {

class GameView;

class MCCFRInfoSet {
private:
    std::vector<uint8_t> d_cardStates;
    std::vector<uint8_t> d_hiddenHands;
    uint8_t d_trump;

public:
    MCCFRInfoSet() = default;
    MCCFRInfoSet(const GameView&);

    bool operator==(const MCCFRInfoSet&) const;

    template <class Archive>
    void serialize(Archive& archive);

    friend struct std::hash<MCCFRInfoSet>;
};

template <class Archive>
inline void MCCFRInfoSet::serialize(Archive& archive)
{
    archive(d_cardStates, d_hiddenHands, d_trump);
}

}

namespace std {

namespace {
    template <typename T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}

template <>
struct hash<jester::MCCFRInfoSet> {
    size_t operator()(const jester::MCCFRInfoSet& is) const
    {
        size_t seed = 1337;
        hash_combine(seed, is.d_trump);
        for (auto& state : is.d_cardStates) {
            hash_combine(seed, state);
        }
        for (auto& hhsz : is.d_hiddenHands) {
            hash_combine(seed, hhsz);
        }
        return seed;
    }
};

}

#endif
