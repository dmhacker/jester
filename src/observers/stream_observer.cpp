#include "stream_observer.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

namespace jester {

namespace {
    template <class T>
    std::string stringifyIterable(T objects)
    {
        std::stringstream ss;
        ss << "[";
        size_t idx = 0;
        auto it = objects.begin();
        for (; it != objects.end(); it++) {
            ss << *it;
            if (idx < objects.size() - 1) {
                ss << ", ";
            }
            idx++;
        }
        ss << "]";
        return ss.str();
    }

    template <bool thin>
    void printBarrier()
    {
        std::cout
            << std::string(60, thin ? '-' : '=')
            << std::endl;
    }

    void printPlayerLine(size_t pid, std::string line)
    {
        std::cout
            << "[P" << pid << "] "
            << line << std::endl;
    }

    void printGameState(const Game& game)
    {
        std::cout << "The game state is now: " << std::endl;
        for (size_t pid = 0; pid < game.playerCount(); pid++) {
            std::cout << "  H" << pid << " -- "
                      << stringifyIterable(game.hand(pid)) 
                      << std::endl;
        }
        std::cout << "  DK -- "
                  << stringifyIterable(game.deck())
                  << std::endl;
    }
}

void StreamObserver::onGameStart(const Game& game)
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

void StreamObserver::onPostAttack(const Game& game, const Action& action)
{
    auto pid = game.attackerId();
    if (action.empty()) {
        assert(!game.currentAttack().empty());
        printPlayerLine(pid, "Choose to halt the attack.");
    } else {
        std::stringstream ss;
        ss << "Attacked with " << action.card() << ".";
        printPlayerLine(pid, ss.str());
    }
}

void StreamObserver::onPostDefend(const Game& game, const Action& action)
{
    auto pid = game.defenderId();
    if (action.empty()) {
        printPlayerLine(pid, "Gave up the defense.");
    } else {
        std::stringstream ss;
        ss << "Defended with " << action.card() << ".";
        printPlayerLine(pid, ss.str());
    }
}

void StreamObserver::onTurnEnd(const Game& game, bool defense_success)
{
    printGameState(game);
    printBarrier<true>();
}

void StreamObserver::onPlayerWin(const Game& game, size_t player_id, size_t win_position)
{
    std::stringstream ss;
    ss << "Finished game in " << win_position;
    if (win_position == 1) {
        ss << "st place.";
    } else if (win_position == 2) {
        ss << "nd place.";
    } else {
        ss << "th place.";
    }
    printPlayerLine(player_id, ss.str());
}

void StreamObserver::onGameEnd(const Game& game)
{
    printBarrier<true>();
    std::cout
        << "The winners in order are "
        << stringifyIterable(game.winOrder())
        << "." << std::endl;
    std::cout
        << "This means that player "
        << game.winOrder().back()
        << " is the loser!" << std::endl;
    printBarrier<false>();
}

}
