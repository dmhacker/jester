#include <catch.hpp>

#include <game/constants.hpp>
#include <game/card.hpp>

using namespace jester;

TEST_CASE("Cank rank, suit, index methods are correct")
{
    for (size_t rank = Constants::instance().MIN_RANK;
         rank <= Constants::instance().MAX_RANK; rank++) {
        std::vector<Card> cards;
        cards.push_back(Card(rank, Suit::hearts));
        cards.push_back(Card(rank, Suit::diamonds));
        cards.push_back(Card(rank, Suit::spades));
        cards.push_back(Card(rank, Suit::clubs));
        REQUIRE(cards[0].suit() == Suit::hearts);
        REQUIRE(cards[1].suit() == Suit::diamonds);
        REQUIRE(cards[2].suit() == Suit::spades);
        REQUIRE(cards[3].suit() == Suit::clubs);
        for (size_t i = 0; i < 4; i++) {
            REQUIRE(cards[i].rank() == rank);
            REQUIRE(cards[i].index() == rank * 4 + i);
        }
    }
}

TEST_CASE("Actions are sorted correctly")
{
    std::vector<Action> actions;
    actions.push_back(Action());
    for (size_t rank = Constants::instance().MIN_RANK;
         rank <= Constants::instance().MAX_RANK; rank++) {
        actions.push_back(Action(Card(rank, Suit::hearts)));
        actions.push_back(Action(Card(rank, Suit::diamonds)));
        actions.push_back(Action(Card(rank, Suit::spades)));
        actions.push_back(Action(Card(rank, Suit::clubs)));
    }
    std::random_shuffle(actions.begin(), actions.end());
    std::sort(actions.begin(), actions.end());
    REQUIRE(actions[0].empty());
    for (size_t i = 1; i < actions.size() - 1; i++) {
        auto card1 = actions[i].card();
        auto card2 = actions[i + 1].card();
        REQUIRE(card1 < card2);
    }
}
