#ifndef JESTER_GAME_ENGINE_HPP
#define JESTER_GAME_ENGINE_HPP

#include "../cfr/cfr_engine.hpp"
#include "../players/player.hpp"

#include <random>
#include <stda/erased_ptr.hpp>
#include <string>

namespace jester {

class PlayerOption {
private:
    std::string d_name;
    std::function<stda::erased_ptr<Player>()> d_producer;

public:
    PlayerOption(const std::string& name, std::function<stda::erased_ptr<Player>()> producer);
    const std::string& name() const;
    stda::erased_ptr<Player> produce() const;
};

class GameEngine {
private:
    CFREngine d_cfr;
    std::vector<PlayerOption> d_options;
    std::mt19937 d_rng;

public:
    GameEngine();
    void shell();

private:
    void printOptions() const;
};

inline const std::string& PlayerOption::name() const
{
    return d_name;
}

inline stda::erased_ptr<Player> PlayerOption::produce() const
{
    return d_producer();
}

}

#endif
