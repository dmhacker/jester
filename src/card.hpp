#ifndef JESTER_CARD_HPP
#define JESTER_CARD_HPP

#include <cstddef>
#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jester {

enum Suit {
    hearts = 0,
    diamonds = 1,
    spades = 2,
    clubs = 3
};

class Card {
private:
    size_t d_rank;
    Suit d_suit;
public:
    Card();
    Card(size_t cardrank, Suit cardsuit);

    size_t rank() const;
    Suit suit() const;
    bool operator==(const Card& card) const;
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;
using AttackSequence = std::vector<Card>;
using DefenseSequence = std::vector<Card>;

inline size_t Card::rank() const
{
    return d_rank;
}

inline Suit Card::suit() const
{
    return d_suit;
}

inline bool Card::operator==(const Card& card) const
{
    return d_rank == card.d_rank && d_suit == card.d_suit;
}

}

namespace std {
template <>
struct hash<jester::Card> {
    size_t operator()(const jester::Card& card) const
    {
        return hash<int>()(card.rank() * 4 + card.suit());
    }
};
}

#endif
