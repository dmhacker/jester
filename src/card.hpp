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
    Card()
        : d_rank(1)
        , d_suit(Suit::hearts)
    {
    }

    Card(size_t cardrank, Suit cardsuit)
        : d_rank(cardrank)
        , d_suit(cardsuit)
    {
    }

    size_t rank() const
    {
        return d_rank;
    }

    Suit suit() const
    {
        return d_suit;
    }

    bool operator==(const Card& card) const
    {
        return d_rank == card.d_rank && d_suit == card.d_suit;
    }
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;
using AttackSequence = std::vector<Card>;
using DefenseSequence = std::vector<Card>;
using DiscardPile = std::unordered_set<Card>;
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
