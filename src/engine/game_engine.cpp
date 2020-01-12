#include "game_engine.hpp"

#include "../rules/game.hpp"

#include "../observers/omniscient_observer.hpp"
#include "../players/dmcts_player.hpp"
#include "../players/greedy_player.hpp"
#include "../players/ismcts_player.hpp"
#include "../players/minimal_player.hpp"
#include "../players/random_player.hpp"
#include "../players/cfrm_player.hpp"

#include <iostream>
#include <sstream>
#include <thread>

namespace jester {
namespace {
    template <bool thin>
    void printBarrier()
    {
        std::cout
            << std::string(60, thin ? '-' : '=')
            << std::endl;
    }

    size_t readInteger()
    {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            throw std::invalid_argument("Please enter a number.");
        }
        size_t num;
        std::istringstream(line) >> num;
        return num;
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
    d_options.push_back(PlayerOption("Prototype CFRM", [](bool has_human) {
        return std::make_shared<CFRMPlayer>(!has_human);
    }));
}

void GameEngine::shell() const
{
    while (std::cin) {
        size_t num_players;
        try {
            std::cout << "Number of players: ";
            num_players = readInteger();
            if (num_players < 2 || num_players > 6) {
                throw std::invalid_argument("The number of players must be between 2 and 6.");
            }
        } catch (std::exception& ex) {
            if (!std::cin) {
                break;
            }
            std::cerr << ex.what() << std::endl;
            continue;
        }

        // Print
        std::cout << std::endl;
        std::cout << "Please choose strategies from the following list." << std::endl;
        printOptions();
        std::cout << "Enter the number corresponding to your desired strategy." << std::endl;
        std::cout << std::endl;

        std::vector<size_t> selections;
        for (size_t i = 1; i <= num_players; i++) {
            if (!std::cin) {
                break;
            }
            std::cout << "Player " << (i - 1) << "'s strategy: ";
            try {
                auto selection = readInteger();
                if (selection < 0 || selection >= d_options.size()) {
                    throw std::invalid_argument("Please choose a number representing a valid strategy.");
                }
                selections.push_back(selection);
            } catch (std::exception& ex) {
                if (!std::cin) {
                    break;
                }
                std::cerr << ex.what() << std::endl;
                i--;
            }
        }
        if (!std::cin) {
            break;
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
        std::cout << std::endl;
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

        std::cout << std::endl;
        printBarrier<false>();
        std::cout << "The game has finished. Select players for the next game." << std::endl;
        printBarrier<false>();
        std::cout << std::endl;
    }
}

void GameEngine::printOptions() const
{
    for (size_t i = 0; i < d_options.size(); i++) {
        std::cout << i << ") " << d_options[i].name() << std::endl;
    }
}

}
