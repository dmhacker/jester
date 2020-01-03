#include <iostream>

#include "player.hpp"
#include "random_player.hpp"
#include "game.hpp"

using namespace jester;

int main() {
    std::vector<std::shared_ptr<Player>> players;
    for (int i = 0; i < 3; i++) {
        players.push_back(std::make_shared<RandomPlayer>());
    }
    Game root(players);
    for (size_t g = 0; g < 3; g++) {
        Game game(root);
        game.play();
        std::cout << "Win order:";
        for (auto & pidx : game.winOrder()) {
            std::cout << " " << pidx;
        }
        std::cout << std::endl;
    }
}
