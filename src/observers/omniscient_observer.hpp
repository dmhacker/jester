#ifndef JESTER_OMNISCIENT_OBSERVER_HPP
#define JESTER_OMNISCIENT_OBSERVER_HPP

#include "observer.hpp"

namespace jester {

class OmniscientObserver : public Observer {
public:
    void onGameStart(const Game& game);
    void onPostAction(const Game& game, const Action& action, bool was_attack);
    void onTurnEnd(const Game& game, bool defense_success);
    void onPlayerWin(const Game& game, size_t player_id, size_t win_position);
    void onGameEnd(const Game& game);
};

}

#endif
