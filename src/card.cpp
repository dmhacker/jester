#include "card.hpp"

namespace jester {

Card::Card()
    : d_rank(1)
    , d_suit(Suit::hearts)
{
}

Card::Card(size_t _rank, Suit _suit)
    : d_rank(_rank)
    , d_suit(_suit)
{
}

std::string to_string(Rank rank)
{
    if (rank == 11) {
        return "J";
    } else if (rank == 12) {
        return "Q";
    } else if (rank == 13) {
        return "K";
    } else if (rank == 14) {
        return "A";
    } else {
        return std::to_string(rank);
    }
}

std::string to_string(Suit suit)
{
    switch (suit) {
    case Suit::hearts: {
        return "♥️";
    }
    case Suit::diamonds: {
        return "♦️";
    }
    case Suit::spades: {
        return "♠️";
    }
    case Suit::clubs: {
        return "♣️";
    }
    }
    return std::string();
}

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    return os << to_string(card.d_rank) << to_string(card.d_suit);
}

Action::Action()
    : d_card(nullptr)
{
}

Action::Action(const Card & _card)
    : d_card(std::make_shared<Card>(_card))
{
}

}
