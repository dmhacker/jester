#include "human_player.hpp"

#include <iostream>
#include <sstream>

namespace jester {

namespace {
    int readInteger()
    {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            throw std::invalid_argument("Please enter a number.");
        }
        int num;
        std::istringstream(line) >> num;
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
            std::cout << "Enter your desired action: ";
            try {
                selection = readInteger();
            } catch (std::exception& ex) {
                if (!std::cin) {
                    break;
                }
                std::cerr << ex.what() << std::endl;
            }
            if (selection < static_cast<int>(view.nextActions().size())
                && selection >= 0) {
                break;
            }
        }
    }
    if (!std::cin) {
        std::cout << "Defaulting to choice 0." << std::endl;
        return view.nextActions()[0];
    }
    return view.nextActions()[selection];
}

}
