#include "omniscient_observer.hpp"
#include "../logging.hpp"
#include "../rules/game_state.hpp"

#include <sstream>

namespace jester {

namespace {
    void printMainBarrier()
    {
        std::cout
            << std::string(60, '=')
            << std::endl;
    }
    void printTurnBarrier(int turn)
    {
        std::cout
            << std::string(28, '-')
            << "[T" << turn << "]"
            << std::string(28, '-')
            << std::endl;
    }
}

void OmniscientObserver::onGameStart(const GameState& state)
{
    printMainBarrier();
    std::cout << "A new game is beginning!" << std::endl;
    std::cout << "There are "
              << state.playerCount()
              << " players in this game." << std::endl;
    std::cout << "Trump card is "
              << state.trumpCard()
              << "." << std::endl;
    std::cout << "The game state is: " << std::endl;
    std::cout << state;
    printTurnBarrier(state.turn());
    std::cout << "[P" << state.attackerId() << "] Beginning attack." << std::endl;
    std::cout << "[P" << state.defenderId() << "] Beginning defense." << std::endl;
}

void OmniscientObserver::onPostAction(const GameState& state, const Action& action, bool was_attack)
{
    auto pid = was_attack ? state.attackerId() : state.defenderId();
    std::cout << "[P" << pid << "] ";
    if (was_attack) {
        if (action.empty()) {
            std::cout << "Halted the attack." << std::endl;
        } else {
            std::cout << "Attacked with " << action.card() << "." << std::endl;
        }
    } else {
        if (action.empty()) {
            std::cout << "Gave up the defense." << std::endl;
        } else {
            std::cout << "Defended with " << action.card() << "." << std::endl;
        }
    }
}

void OmniscientObserver::onHandReplenish(const GameState& state, size_t pid, const Card& card) {
    std::cout << "[P" << pid << "] Drew card " << card << "." << std::endl;
}

void OmniscientObserver::onTurnEnd(const GameState& state, bool defense_success)
{
    std::cout << "The game state is now: " << std::endl;
    std::cout << state;
    printTurnBarrier(state.turn());
    std::cout << "[P" << state.attackerId() << "] Beginning attack." << std::endl;
    std::cout << "[P" << state.defenderId() << "] Beginning defense." << std::endl;
}

void OmniscientObserver::onPlayerWin(const GameState& state, size_t player_id, size_t win_position)
{
    std::cout << "[P" << player_id
              << "] Finished game in " << win_position;
    if (win_position == 1) {
        std::cout << "st place.";
    } else if (win_position == 2) {
        std::cout << "nd place.";
    } else if (win_position == 3) {
        std::cout << "rd place.";
    } else {
        std::cout << "th place.";
    }
    std::cout << std::endl;
}

void OmniscientObserver::onGameEnd(const GameState& state)
{
    printTurnBarrier(state.turn());
    std::cout
        << "The winners in order are "
        << state.winOrder()
        << "." << std::endl;
    std::cout
        << "Player "
        << state.winOrder().back()
        << " is the loser!" << std::endl;
    printMainBarrier();
}

}
