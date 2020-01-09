#include "game_engine.hpp"

#include "../rules/game.hpp"

#include "../observers/omniscient_observer.hpp"
#include "../players/dmcts_player.hpp"
#include "../players/greedy_player.hpp"
#include "../players/ismcts_player.hpp"
#include "../players/minimal_player.hpp"
#include "../players/random_player.hpp"

#include <iostream>
#include <sstream>
#include <thread>

namespace jester {
namespace {
    std::vector<std::string> splitString(const std::string& strn, const std::string& delimiter)
    {
        std::vector<std::string> split;
        auto start = 0U;
        auto end = strn.find(delimiter);
        while (end != std::string::npos) {
            split.push_back(strn.substr(start, end - start));
            start = end + delimiter.length();
            end = strn.find(delimiter, start);
        }
        split.push_back(strn.substr(start, end - start));
        return split;
    }

    std::vector<size_t> translateIntegers(const std::vector<std::string>& split)
    {
        std::vector<size_t> selections;
        for (auto& token : split) {
            size_t num;
            std::istringstream(token) >> num;
            selections.push_back(num);
        }
        return selections;
    }
}

PlayerOption::PlayerOption(const std::string& name,
    std::function<std::shared_ptr<Player>(bool)> producer)
    : d_name(name)
    , d_producer(producer)
{
}

GameEngine::GameEngine()
{
    d_options.push_back(PlayerOption("Minimal", [](bool has_human) {
        return std::make_shared<MinimalPlayer>();
    }));
    d_options.push_back(PlayerOption("Random", [](bool has_human) {
        return std::make_shared<RandomPlayer>();
    }));
    d_options.push_back(PlayerOption("Greedy", [](bool has_human) {
        return std::make_shared<GreedyPlayer>();
    }));
    d_options.push_back(PlayerOption("Weak DMCTS", [](bool has_human) {
        return std::make_shared<DMCTSPlayer>(!has_human, 3, 1, std::chrono::milliseconds(4000));
    }));
    d_options.push_back(PlayerOption("Weak ISMCTS", [](bool has_human) {
        return std::make_shared<ISMCTSPlayer>(!has_human, 1, std::chrono::milliseconds(4000));
    }));
    d_options.push_back(PlayerOption("Strong DMCTS", [](bool has_human) {
        auto cores = std::thread::hardware_concurrency();
        return std::make_shared<DMCTSPlayer>(!has_human, cores + 2, cores, std::chrono::milliseconds(9000));
    }));
    d_options.push_back(PlayerOption("Strong ISMCTS", [](bool has_human) {
        auto cores = std::thread::hardware_concurrency();
        return std::make_shared<ISMCTSPlayer>(!has_human, cores, std::chrono::milliseconds(9000));
    }));
}

void GameEngine::shell() const
{
    shellMessage();

    while (std::cin) {
        shellPrompt();

        // Get the input from the user
        std::string line;
        std::getline(std::cin, line);
        if (line.size() == 0) {
            continue;
        }

        // Turn the input into a list of integers (list of selections)
        std::vector<size_t> selections = translateIntegers(splitString(line, ","));
        if (selections.empty()) {
            std::cerr << "Your input was formatted incorrectly." << std::endl;
            continue;
        }
        if (selections.size() == 1) {
            std::cerr << "There must be at least 2 players in the game." << std::endl;
            continue;
        }

        // Parse the selections into player objects
        bool has_humans = false;
        bool invalid_player = false;
        std::vector<std::shared_ptr<Player>> players;
        std::vector<std::string> player_types;
        for (auto& selection : selections) {
            if (selection < 0 || selection >= d_options.size()) {
                std::cerr << "Selection " << selection << " is invalid." << std::endl;
                invalid_player = true;
                break;
            }
            auto& option = d_options[selection];
            players.push_back(option.produce(has_humans));
            player_types.push_back(option.name());
        }
        if (invalid_player) {
            continue;
        }

        // Print out table mapping IDs to player types
        std::cout
            << "ID\tPlayer" << std::endl
            << "--\t------" << std::endl;
        for (size_t i = 0; i < player_types.size(); i++) {
            std::cout << i << "\t" << player_types[i] << std::endl;
        }
        std::cout << std::endl;

        // Play out the game
        Game game(players);
        if (!has_humans) {
            game.registerObserver(std::make_shared<OmniscientObserver>());
        }
        game.play();

        // Add message explaining how to set up the next game
        shellMessage();
    }
}

void GameEngine::shellPrompt() const
{
    std::cout << ">>> ";
}

void GameEngine::shellMessage() const
{
    std::cout << std::endl;
    std::cout
        << "Use the following prompt to specify what"
        << " players should be included in the next game." 
        << std::endl
        << "You will be given a numbered list of options."
        << std::endl
        << "List selected options in your desired order,"
        << " separating them via commas."
        << std::endl
        << "For example, entering \"1,0\" would"
        << " create a game with a minimal player 0 and"
        << " random player 1."
        << std::endl
        << std::endl;
    for (size_t i = 0; i < d_options.size(); i++) {
        std::cout << i << ") " << d_options[i].name() << std::endl;
    }
    std::cout << std::endl;
}

}
