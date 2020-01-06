#ifndef JESTER_CARD_HPP
#define JESTER_CARD_HPP

#include <cstddef>
#include <deque>
#include <functional>
#include <iostream>
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

class Action {
private:
    std::shared_ptr<Card> d_card;

public:
    Action();
    Action(const Card& _card);

    bool empty() const;
    const Card& card() const;
    bool operator==(const Action& action) const;

private:
    friend std::ostream& operator<<(std::ostream& os, const Action& action);
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;
using CardPile = std::unordered_set<Card>;
using CardSequence = std::vector<Card>;

std::string toString(Rank rank);
std::string toString(Suit rank);

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

inline bool Action::operator==(const Action& action) const
{
    if (empty()) {
        return action.empty();
    }
    if (action.empty()) {
        return false;
    }
    return card() == action.card();
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

template <>
struct hash<jester::Action> {
    size_t operator()(const jester::Action& action) const
    {
        if (action.empty()) {
            return hash<int>()(0);
        }
        return hash<jester::Card>()(action.card());
    }
};

}

#endif
