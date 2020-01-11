#ifndef JESTER_CFRM_ABSTRACTION_HPP
#define JESTER_CFRM_ABSTRACTION_HPP

#include <memory>
#include <cereal/types/vector.hpp>

namespace jester {

class CFRMAbstraction;

}

namespace std {

template <>
struct hash<jester::CFRMAbstraction>;

}

namespace jester {

class GameView;

class CFRMAbstraction {
private:
    std::vector<uint8_t> d_cardStates;
    std::vector<uint8_t> d_hiddenHands;
    uint8_t d_trump;

public:
    CFRMAbstraction() = default;
    CFRMAbstraction(const GameView&);

    bool operator==(const CFRMAbstraction&) const;

    template <class Archive>
    void serialize(Archive& archive);

    friend struct std::hash<CFRMAbstraction>;

private:
    friend std::ostream& operator<<(std::ostream&, const CFRMAbstraction&);
};

template <class Archive>
inline void CFRMAbstraction::serialize(Archive& archive)
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
struct hash<jester::CFRMAbstraction> {
    size_t operator()(const jester::CFRMAbstraction& abstraction) const
    {
        size_t seed = 1337;
        hash_combine(seed, abstraction.d_trump);
        for (auto& state : abstraction.d_cardStates) {
            hash_combine(seed, state);
        }
        for (auto& hhsz : abstraction.d_hiddenHands) {
            hash_combine(seed, hhsz);
        }
        return seed;
    }
};

}

#endif
