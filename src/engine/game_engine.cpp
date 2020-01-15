#include "game_engine.hpp"

#include "../constants.hpp"
#include "../observers/omniscient_observer.hpp"
#include "../players/cfrm_player.hpp"
#include "../players/dmcts_player.hpp"
#include "../players/greedy_player.hpp"
#include "../players/ismcts_player.hpp"
#include "../players/minimal_player.hpp"
#include "../players/random_player.hpp"
#include "../players/human_player.hpp"
#include "../rules/game_state.hpp"

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

PlayerOption::PlayerOption(const std::string& name,
    std::function<stda::erased_ptr<Player>(bool)> producer)
    : d_name(name)
    , d_producer(producer)
{
}

GameEngine::GameEngine()
    : d_rng(std::random_device {}())
{
    d_options.push_back(PlayerOption("Human", [](bool has_human) {
        return stda::make_erased<HumanPlayer>();
    }));
    d_options.push_back(PlayerOption("Minimal", [](bool has_human) {
        return stda::make_erased<MinimalPlayer>();
    }));
    d_options.push_back(PlayerOption("Random", [](bool has_human) {
        return stda::make_erased<RandomPlayer>();
    }));
    d_options.push_back(PlayerOption("Greedy", [](bool has_human) {
        return stda::make_erased<GreedyPlayer>();
    }));
    d_options.push_back(PlayerOption("Weak DMCTS", [](bool has_human) {
        return stda::make_erased<DMCTSPlayer>(!has_human, 3, 1, std::chrono::seconds(4));
    }));
    d_options.push_back(PlayerOption("Weak ISMCTS", [](bool has_human) {
        return stda::make_erased<ISMCTSPlayer>(!has_human, 1, std::chrono::seconds(4));
    }));
    d_options.push_back(PlayerOption("Strong DMCTS", [](bool has_human) {
        auto cores = std::thread::hardware_concurrency();
        return stda::make_erased<DMCTSPlayer>(!has_human, cores + 2, cores, std::chrono::seconds(9));
    }));
    d_options.push_back(PlayerOption("Strong ISMCTS", [](bool has_human) {
        auto cores = std::thread::hardware_concurrency();
        return stda::make_erased<ISMCTSPlayer>(!has_human, cores, std::chrono::seconds(9));
    }));
    d_options.push_back(PlayerOption("Tabular CFRM", [](bool has_human) {
        return stda::make_erased<CFRMPlayer>(!has_human);
    }));
}

void GameEngine::shell()
{
    while (std::cin) {
        // Have the user choose the number of players in the game
        size_t num_players;
        try {
            std::cout << "Number of players: ";
            num_players = readInteger();
            if (num_players < Constants::instance().MIN_PLAYERS
                || num_players > Constants::instance().MAX_PLAYERS) {
                std::stringstream ss;
                ss << "The number of players must be between ";
                ss << Constants::instance().MIN_PLAYERS;
                ss << " and ";
                ss << Constants::instance().MAX_PLAYERS;
                ss << ".";
                throw std::invalid_argument(ss.str());
            }
        } catch (std::exception& ex) {
            if (!std::cin) {
                break;
            }
            std::cerr << ex.what() << std::endl;
            continue;
        }

        // Have the users choose which player types they will use
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
                if (selection < 0 || selection >= static_cast<int>(d_options.size())) {
                    std::stringstream ss;
                    ss << "Selection " << selection << " is invalid.";
                    throw std::invalid_argument(ss.str());
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

        // Convert selections into player objects
        bool has_humans = false;
        std::vector<stda::erased_ptr<Player>> players;
        std::vector<std::string> player_types;
        for (auto& selection : selections) {
            auto& option = d_options[selection];
            players.push_back(option.produce(has_humans));
            player_types.push_back(option.name());
            if (option.name() == "Human") {
                has_humans = false;
            }
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

        // Set up a game instance
        GameState game(players.size(), d_rng);
        if (!has_humans) {
            game.setObserver(stda::make_erased<OmniscientObserver>());
        }

        // Play out the game to conclusion
        while (!game.finished()) {
            auto& player = players[game.currentPlayerId()];
            auto action = player->nextAction(game.currentPlayerView());
            game.validateAction(action);
            game.playAction(action);
        }

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
