#include "rules/game.hpp"
#include "players/random_player.hpp"
#include "players/minimal_player.hpp"
#include "observers/omniscient_observer.hpp"

using namespace jester;

int main() {
    std::vector<std::shared_ptr<Player>> players;
    players.push_back(std::make_shared<RandomPlayer>());
    players.push_back(std::make_shared<RandomPlayer>());
    players.push_back(std::make_shared<MinimalPlayer>());
    players.push_back(std::make_shared<RandomPlayer>());
    players.push_back(std::make_shared<RandomPlayer>());
    Game game(players);
    game.registerObserver(std::make_shared<OmniscientObserver>());
    game.play();
}
