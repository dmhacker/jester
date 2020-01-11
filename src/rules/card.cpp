#include "card.hpp"

namespace jester {

Card::Card()
    : d_rank(1)
    , d_suit(Suit::hearts)
{
}

Card::Card(uint8_t rank, Suit suit)
    : d_rank(rank)
    , d_suit(suit)
{
}

Action::Action()
    : d_card(0, Suit::hearts)
{
}

Action::Action(const Card& card)
    : d_card(card)
{
}

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    return os << toString(card.d_rank) << toString(card.d_suit);
}

std::ostream& operator<<(std::ostream& os, const Action& action)
{
    if (action.empty()) {
        return os << "Yield";
    } else {
        return os << "Use " << action.card();
    }
}

std::string toString(Rank rank)
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

std::string toString(Suit suit)
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

Card toCard(uint8_t index)
{
    return Card(index / 4 + 6, static_cast<Suit>(index % 4));
}

}
