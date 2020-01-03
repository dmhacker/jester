#include "card.hpp"

namespace jester{

Card::Card()
    : d_rank(1)
    , d_suit(Suit::hearts)
{
}

Card::Card(size_t cardrank, Suit cardsuit)
    : d_rank(cardrank)
    , d_suit(cardsuit)
{
}

}
