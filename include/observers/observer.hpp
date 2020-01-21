#ifndef JESTER_OBSERVER_HPP
#define JESTER_OBSERVER_HPP

#include "../game/game_state.hpp"

namespace jester {

class Observer {
public:
    virtual void onGameStart(const GameState& state) = 0;
    virtual void onPostAction(const GameState& state, const Action& action, bool was_attack) = 0;
    virtual void onTurnEnd(const GameState& state, bool defense_success) = 0;
    virtual void onHandReplenish(const GameState& state, size_t pid, const Card& card) = 0;
    virtual void onPlayerWin(const GameState& state, size_t pid, size_t win_position) = 0;
    virtual void onGameEnd(const GameState& state) = 0;
};

}

#endif
