#include <jester/players/human_player.hpp>
#include <jester/utils/logging.hpp>

#include <iostream>
#include <sstream>

namespace jester {

namespace {
    int readInteger(int minimum, int maximum)
    {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            std::stringstream ess;
            ess << "Please enter a number between "
                << minimum << " and "
                << maximum << ".";
            throw std::invalid_argument(ess.str());
        }
        int num;
        std::istringstream(line) >> num;
        if (num < minimum || num > maximum) {
            std::stringstream ess;
            ess << "Please enter a number between "
                << minimum << " and "
                << maximum << ".";
            throw std::invalid_argument(ess.str());
        }
        return num;
    }
}

Action HumanPlayer::nextAction(const GameView& view)
{
    std::cout << std::string(44, '+') << std::endl;
    std::cout << "You are playing as P" << view.playerId();
    if (view.attackerNext()) {
        std::cout
            << " and are attacking P" << view.defenderId()
            << "." << std::endl;
        ;
    } else {
        std::cout
            << " and are defending against P" << view.attackerId()
            << "." << std::endl;
        ;
    }
    for (size_t pid = 0; pid < view.playerCount(); pid++) {
        if (pid == view.playerId()) {
            std::cout << "  Your hand is " << view.hand()
                      << "." << std::endl;
        } else {
            std::cout << "  P" << pid
                      << "'s hand is " << view.visibleHand(pid)
                      << " with " << view.hiddenHandSize(pid)
                      << " hidden cards." << std::endl;
        }
    }
    std::cout
        << "  Current attack is " << view.currentAttack()
        << "." << std::endl;
    std::cout
        << "  Current defense is " << view.currentDefense()
        << "." << std::endl;
    std::cout
        << "  There are " << view.deckSize()
        << " cards left in the deck." << std::endl;
    std::cout
        << "Select a valid move to make from this list."
        << std::endl;
    for (size_t i = 0; i < view.nextActions().size(); i++) {
        std::cout << "  " << i << ") " << view.nextActions()[i] << "." << std::endl;
    }
    int selection = 0;
    std::cout << "Your move: ";
    while (true) {
        if (std::cin) {
            try {
                selection = readInteger(0, view.nextActions().size() - 1);
                break;
            } catch (std::exception& ex) {
                if (!std::cin) {
                    break;
                }
                std::cerr << ex.what() << std::endl;
            }
        }
    }
    if (!std::cin) {
        exit(EXIT_SUCCESS);
    }
    std::cout << std::string(44, '+') << std::endl;
    return view.nextActions()[selection];
}

}
