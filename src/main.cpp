#include "rules/game.hpp"
#include "players/minimal_player.hpp"
#include "players/random_player.hpp"
#include "players/greedy_player.hpp"
#include "players/dmcts_player.hpp"
#include "players/ismcts_player.hpp"
#include "observers/omniscient_observer.hpp"

using namespace jester;

int main() {
    std::vector<std::shared_ptr<Player>> players;
    players.push_back(std::make_shared<ISMCTSPlayer>(true));
    /* players.push_back(std::make_shared<GreedyPlayer>()); */
    players.push_back(std::make_shared<DMCTSPlayer>(true));
    Game game(players);
    game.registerObserver(std::make_shared<OmniscientObserver>());
    game.play();
}
