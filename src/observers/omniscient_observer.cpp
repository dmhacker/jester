#include "omniscient_observer.hpp"

#include "../rules/game.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

namespace jester {

namespace {
    template <bool thin>
    void printBarrier()
    {
        std::cout
            << std::string(60, thin ? '-' : '=')
            << std::endl;
    }
}

void OmniscientObserver::onGameStart(const Game& game)
{
    printBarrier<false>();
    std::cout << "A new game is beginning!" << std::endl;
    std::cout << "There are "
              << game.playerCount()
              << " players in this game." << std::endl;
    std::cout << "Trump card is "
              << game.trumpCard()
              << "." << std::endl;
    printBarrier<true>();
}

void OmniscientObserver::onPostAction(const Game& game, const Action& action, bool was_attack)
{
    auto pid = was_attack ? game.attackerId() : game.defenderId();
    std::cout << "[GAME] Player " << pid << " ";
    if (was_attack) {
        if (action.empty()) {
            assert(!game.currentAttack().empty());
            std::cout << "choose to halt the attack." << std::endl;
        } else {
            std::cout << "attacked with " << action.card() << "." << std::endl;
        }
    } else {
        if (action.empty()) {
            std::cout << "gave up the defense." << std::endl;
        } else {
            std::cout << "defended with " << action.card() << "." << std::endl;
        }
    }
}

void OmniscientObserver::onTurnEnd(const Game& game, bool defense_success)
{
    std::cout << "The game state is now: " << std::endl;
    std::cout << game;
    printBarrier<true>();
}

void OmniscientObserver::onPlayerWin(const Game& game, size_t player_id, size_t win_position)
{
    std::cout << "[GAME] Player " << player_id
              << " finished game in " << win_position;
    if (win_position == 1) {
        std::cout << "st place.";
    } else if (win_position == 2) {
        std::cout << "nd place.";
    } else {
        std::cout << "th place.";
    }
    std::cout << std::endl;
}

void OmniscientObserver::onGameEnd(const Game& game)
{
    printBarrier<true>();
    std::cout
        << "The winners in order are "
        << game.winOrder()
        << "." << std::endl;
    std::cout
        << "Player "
        << game.winOrder().back()
        << " is the loser!" << std::endl;
    printBarrier<false>();
}

}
