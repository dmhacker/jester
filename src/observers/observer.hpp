#ifndef JESTER_OBSERVER_HPP
#define JESTER_OBSERVER_HPP

#include "../rules/game.hpp"

namespace jester {

class Observer {
public:
    virtual void onGameStart(const Game& game) = 0;
    virtual void onPostAction(const Game& game, const Action& action, bool was_attack) = 0;
    virtual void onTurnEnd(const Game& game, bool defense_success) = 0;
    virtual void onPlayerWin(const Game& game, size_t player_id, size_t win_position) = 0;
    virtual void onGameEnd(const Game& game) = 0;
};

}

#endif
