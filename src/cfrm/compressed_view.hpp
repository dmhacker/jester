#ifndef COMPRESSED_VIEW_HPP
#define COMPRESSED_VIEW_HPP

#include "../rules/game_view.hpp"

#include <memory>
#include <sstream>

namespace jester {
class CompressedView;
}

namespace std {
template <>
struct hash<jester::CompressedView>;
}

namespace jester {

const static uint8_t MAX_CARDS = 36;
const static uint8_t MAX_PLAYERS = 6;

class CompressedView {
private:
    uint8_t d_cardStates[MAX_CARDS];
    uint8_t d_hiddenHands[MAX_PLAYERS];
    uint8_t d_trump;
    bool d_attacking;

public:
    CompressedView(const GameView& view);

    bool operator==(const CompressedView& cv) const;
    friend struct std::hash<CompressedView>;
};

}

namespace std {

template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <>
struct hash<jester::CompressedView> {
    size_t operator()(const jester::CompressedView& cv) const
    {
        size_t seed = 1337;
        hash_combine(seed, cv.d_attacking);
        hash_combine(seed, cv.d_trump);
        for (size_t i = 0; i < jester::MAX_PLAYERS; i++) {
            hash_combine(seed, cv.d_hiddenHands[i]);
        }
        for (size_t i = 0; i < jester::MAX_CARDS; i++) {

            hash_combine(seed, cv.d_cardStates[i]);
        }
        return seed;
    }
};

}

#endif
