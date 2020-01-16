#include "omniscient_observer.hpp"
#include "../logging.hpp"
#include "../rules/game_state.hpp"

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

void OmniscientObserver::onGameStart(const GameState& state)
{
    printBarrier<false>();
    std::cout << "A new game is beginning!" << std::endl;
    std::cout << "There are "
              << state.playerCount()
              << " players in this game." << std::endl;
    std::cout << "Trump card is "
              << state.trumpCard()
              << "." << std::endl;
    std::cout << "The game state is: " << std::endl;
    std::cout << state;
    printBarrier<true>();
}

void OmniscientObserver::onPostAction(const GameState& state, const Action& action, bool was_attack)
{
    auto pid = was_attack ? state.attackerId() : state.defenderId();
    std::cout << "[GAME] Player " << pid << " ";
    if (was_attack) {
        if (action.empty()) {
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

void OmniscientObserver::onTurnEnd(const GameState& state, bool defense_success)
{
    std::cout << "The game state is now: " << std::endl;
    std::cout << state;
    printBarrier<true>();
}

void OmniscientObserver::onPlayerWin(const GameState& state, size_t player_id, size_t win_position)
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

void OmniscientObserver::onGameEnd(const GameState& state)
{
    printBarrier<true>();
    std::cout
        << "The winners in order are "
        << state.winOrder()
        << "." << std::endl;
    std::cout
        << "Player "
        << state.winOrder().back()
        << " is the loser!" << std::endl;
    printBarrier<false>();
}

}
