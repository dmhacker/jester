#ifndef JESTER_GAME_ENGINE_HPP
#define JESTER_GAME_ENGINE_HPP

#include "../players/player.hpp"

#include <string>
#include <random>
#include <erased_ptr.hpp>

namespace jester {

class PlayerOption {
private:
    std::string d_name;
    std::function<stda::erased_ptr<Player>(bool)> d_producer;

public:
    PlayerOption(const std::string& name, std::function<stda::erased_ptr<Player>(bool)> producer);
    const std::string& name() const;
    stda::erased_ptr<Player> produce(bool has_human) const;
};

class GameEngine {
private:
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

inline stda::erased_ptr<Player> PlayerOption::produce(bool has_human) const
{
    return d_producer(has_human);
}

}

#endif
