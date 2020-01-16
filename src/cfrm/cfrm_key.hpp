#ifndef JESTER_CFRM_KEY_HPP
#define JESTER_CFRM_KEY_HPP

#include <memory>
#include <cereal/types/vector.hpp>

namespace jester {

class CFRMKey;

}

namespace std {

template <>
struct hash<jester::CFRMKey>;

}

namespace jester {

class GameView;

class CFRMKey {
private:
    std::vector<uint8_t> d_cardStates;
    std::vector<uint8_t> d_hiddenHands;
    uint8_t d_trump;

public:
    CFRMKey() = default;
    CFRMKey(const GameView&);

    bool operator==(const CFRMKey&) const;

    template <class Archive>
    void serialize(Archive& archive);

    friend struct std::hash<CFRMKey>;
};

template <class Archive>
inline void CFRMKey::serialize(Archive& archive)
{
    archive(d_cardStates, d_hiddenHands, d_trump);
}

}

namespace std {

template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <>
struct hash<jester::CFRMKey> {
    size_t operator()(const jester::CFRMKey& key) const
    {
        size_t seed = 1337;
        hash_combine(seed, key.d_trump);
        for (auto& state : key.d_cardStates) {
            hash_combine(seed, state);
        }
        for (auto& hhsz : key.d_hiddenHands) {
            hash_combine(seed, hhsz);
        }
        return seed;
    }
};

}

#endif
