#ifndef JESTER_CARD_HPP
#define JESTER_CARD_HPP

#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
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
using Rank = size_t;

class Card {
private:
    Rank d_rank;
    Suit d_suit;

public:
    Card();
    Card(Rank _rank, Suit _suit);

    Rank rank() const;
    Suit suit() const;
    bool operator==(const Card& card) const;

private:
    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;

class Action {
private:
    std::shared_ptr<Card> d_card;

public:
    Action();
    Action(const Card& _card);

    bool empty() const;
    const Card& card() const;
};

std::string to_string(Rank rank);
std::string to_string(Suit rank);

inline Rank Card::rank() const
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

inline bool Action::empty() const
{
    return d_card == nullptr;
}

inline const Card& Action::card() const
{
    return *d_card;
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
