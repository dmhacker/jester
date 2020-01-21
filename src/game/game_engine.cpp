#include <game/constants.hpp>
#include <game/game_engine.hpp>
#include <game/game_state.hpp>
#include <observers/omniscient_observer.hpp>
#include <observers/reduced_observer.hpp>
#include <players/dmcts_player.hpp>
#include <players/greedy_player.hpp>
#include <players/human_player.hpp>
#include <players/ismcts_player.hpp>
#include <players/cfr_player.hpp>
#include <players/minimal_player.hpp>
#include <players/random_player.hpp>

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

PlayerOption::PlayerOption(const std::string& name,
    std::function<stda::erased_ptr<Player>()> producer)
    : d_name(name)
    , d_producer(producer)
{
}

GameEngine::GameEngine()
    : d_rng(std::random_device {}())
{
    d_options.push_back(PlayerOption("Human", []() {
        return stda::make_erased<HumanPlayer>();
    }));
    d_options.push_back(PlayerOption("Minimal", []() {
        return stda::make_erased<MinimalPlayer>();
    }));
    d_options.push_back(PlayerOption("Random", []() {
        return stda::make_erased<RandomPlayer>();
    }));
    d_options.push_back(PlayerOption("Greedy", []() {
        return stda::make_erased<GreedyPlayer>();
    }));
    d_options.push_back(PlayerOption("Weak DMCTS", []() {
        return stda::make_erased<DMCTSPlayer>(3, 1, std::chrono::seconds(4));
    }));
    d_options.push_back(PlayerOption("Weak ISMCTS", []() {
        return stda::make_erased<ISMCTSPlayer>(1, std::chrono::seconds(4));
    }));
    d_options.push_back(PlayerOption("Strong DMCTS", []() {
        auto cores = std::thread::hardware_concurrency();
        return stda::make_erased<DMCTSPlayer>(cores + 2, cores, std::chrono::seconds(9));
    }));
    d_options.push_back(PlayerOption("Strong ISMCTS", []() {
        auto cores = std::thread::hardware_concurrency();
        return stda::make_erased<ISMCTSPlayer>(cores, std::chrono::seconds(9));
    }));
    d_options.push_back(PlayerOption("Tabular CFR", []() {
        return stda::make_erased<CFRPlayer>();
    }));
}

void GameEngine::shell()
{
    while (std::cin) {
        // Have the user choose the number of players in the game
        size_t num_players;
        try {
            std::cout << "Number of players: ";
            num_players = readInteger(Constants::instance().MIN_PLAYERS,
                Constants::instance().MAX_PLAYERS);
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
                auto selection = readInteger(0, d_options.size() - 1);
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
            players.push_back(option.produce());
            player_types.push_back(option.name());
            if (option.name() == "Human") {
                has_humans = true;
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
        if (has_humans) {
            game.setObserver(stda::make_erased<ReducedObserver>());
        } else {
            game.setObserver(stda::make_erased<OmniscientObserver>());
        }

        // Play out the game to conclusion
        game.observer()->onGameStart(game);
        while (!game.finished()) {
            auto& player = players[game.currentPlayerId()];
            auto action = player->nextAction(game.currentPlayerView());
            game.validateAction(action);
            game.playAction(action);
        }
        game.observer()->onGameEnd(game);

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
        std::cout << "  " << i << ") " << d_options[i].name() << std::endl;
    }
}

}
