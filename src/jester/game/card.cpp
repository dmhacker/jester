#include <jester/game/card.hpp>

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
    auto rank = card.d_rank;
    if (rank == 11) {
        os << "J";
    } else if (rank == 12) {
        os << "Q";
    } else if (rank == 13) {
        os << "K";
    } else if (rank == 14) {
        os << "A";
    } else {
        os << static_cast<size_t>(rank);
    }
    switch (card.d_suit) {
    case Suit::hearts: {
        os << "♥️";
        break;
    }
    case Suit::diamonds: {
        os << "♦️";
        break;
    }
    case Suit::spades: {
        os << "♠️";
        break;
    }
    case Suit::clubs: {
        os << "♣️";
        break;
    }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Action& action)
{
    if (action.empty()) {
        return os << "Yield";
    } else {
        return os << "Use " << action.card();
    }
}

}
