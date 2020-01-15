#ifndef JESTER_CARD_HPP
#define JESTER_CARD_HPP

#include <cstddef>
#include <deque>
#include <functional>

#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>

namespace jester {

enum Suit : uint8_t {
    hearts = 0,
    diamonds = 1,
    spades = 2,
    clubs = 3
};
using Rank = uint8_t;

class Card {
private: Rank d_rank; Suit d_suit;

public:
    Card();
    Card(Rank rank, Suit suit);

    uint8_t index() const;
    Rank rank() const;
    Suit suit() const;
    bool operator==(const Card& card) const;
    bool operator<(const Card& card) const;

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};

class Action {
private:
    Card d_card;

public:
    Action();
    Action(const Card& card);

    bool empty() const;
    const Card& card() const;
    bool operator==(const Action& action) const;
    bool operator<(const Action& action) const;

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream& os, const Action& action);
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;
using CardPile = std::unordered_set<Card>;
using CardSequence = std::vector<Card>;

inline Rank Card::rank() const
{
    return d_rank;
}

inline Suit Card::suit() const
{
    return d_suit;
}

inline uint8_t Card::index() const
{
    return d_rank * 4 + d_suit;
}

inline bool Card::operator==(const Card& card) const
{
    return d_rank == card.d_rank && d_suit == card.d_suit;
}

inline bool Card::operator<(const Card& card) const
{
    return index() < card.index();
}

template <class Archive>
inline void Card::serialize(Archive& archive)
{
    archive(d_rank, d_suit);
}

inline bool Action::empty() const
{
    return d_card.rank() == 0;
}

inline const Card& Action::card() const
{
    return d_card;
}

inline bool Action::operator==(const Action& action) const
{
    if (empty()) {
        return action.empty();
    }
    if (action.empty()) {
        return false;
    }
    return d_card == action.d_card;
}

inline bool Action::operator<(const Action& action) const
{
    return d_card.index() < action.d_card.index();
}

template <class Archive>
inline void Action::serialize(Archive& archive)
{
    archive(d_card);
}

}

namespace std {

template <>
struct hash<jester::Card> {
    size_t operator()(const jester::Card& card) const
    {
        return hash<int>()(card.index() + 1);
    }
};

template <>
struct hash<jester::Action> {
    size_t operator()(const jester::Action& action) const
    {
        return hash<jester::Card>()(action.card());
    }
};

}

#endif
