#ifndef JESTER_OMNISCIENT_OBSERVER_HPP
#define JESTER_OMNISCIENT_OBSERVER_HPP

#include "observer.hpp"

namespace jester {

class OmniscientObserver : public Observer {
public:
    void onGameStart(const GameState& state);
    void onPostAction(const GameState& state, const Action& action, bool was_attack);
    void onTurnEnd(const GameState& state, bool defense_success);
    void onPlayerWin(const GameState& state, size_t player_id, size_t win_position);
    void onGameEnd(const GameState& state);
};

}

#endif
