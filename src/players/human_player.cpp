#include "human_player.hpp"

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
    for (size_t i = 0; i < view.nextActions().size(); i++) {
        std::cout << i << ") " << view.nextActions()[i] << std::endl;
    }
    int selection = 0;
    if (std::cin) {
        while (true) {
            std::cout << "Your desired action: ";
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
    return view.nextActions()[selection];
}

}
