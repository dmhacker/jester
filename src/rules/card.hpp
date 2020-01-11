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

enum Suit : uint8_t {
    hearts = 0,
    diamonds = 1,
    spades = 2,
    clubs = 3
};
using Rank = uint8_t;

class Card {
private:
    Rank d_rank;
    Suit d_suit;

public:
    Card();
    Card(Rank _rank, Suit _suit);

    uint8_t index() const;
    Rank rank() const;
    Suit suit() const;
    bool operator==(const Card& card) const;

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

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend std::ostream& operator<<(std::ostream& os, const Action& action);
};

using Deck = std::deque<Card>;
using Hand = std::unordered_set<Card>;
using CardPile = std::unordered_set<Card>;
using CardSequence = std::vector<Card>;

std::string toString(Rank rank);
std::string toString(Suit rank);
Card toCard(uint8_t index);

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
    return (d_rank - 6) * 4 + d_suit;
}

inline bool Card::operator==(const Card& card) const
{
    return d_rank == card.d_rank && d_suit == card.d_suit;
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
    return card() == action.card();
}

template <class Archive>
inline void Action::serialize(Archive& archive)
{
    archive(d_card);
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container)
{
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& container)
{
    os << "[";
    for (size_t i = 0; i < container.size(); i++) {
        os << container[i];
        if (i < container.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& container)
{
    os << "{";
    size_t i = 0;
    for (auto& item : container) {
        os << item;
        if (i < container.size() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "}";
    return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& container)
{
    os << "{";
    size_t i = 0;
    for (auto& item : container) {
        os << item.first << " : " << item.second;
        if (i < container.size() - 1) {
            os << ", ";
        }
        i++;
    }
    os << "}";
    return os;
}

}

namespace std {

template <>
struct hash<jester::Card> {
    size_t operator()(const jester::Card& card) const
    {
        return hash<int>()(card.rank() * 4 + card.suit() + 1);
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
