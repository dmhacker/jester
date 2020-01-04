#include <iostream>

#include "durak/player.hpp"
#include "durak/game.hpp"
#include "players/random_player.hpp"
#include "players/minimal_player.hpp"

using namespace jester;

int main() {
    std::vector<std::shared_ptr<Player>> players;
    players.push_back(std::make_shared<RandomPlayer>());
    players.push_back(std::make_shared<MinimalPlayer>());
    players.push_back(std::make_shared<RandomPlayer>());
    players.push_back(std::make_shared<RandomPlayer>());
    Game root(players);
    for (size_t g = 0; g < 2; g++) {
        Game game(root);
        game.play();
        std::cout << "Win order:";
        for (auto & pidx : game.winOrder()) {
            std::cout << " " << pidx;
        }
        std::cout << std::endl;
    }
}
