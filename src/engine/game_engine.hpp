#ifndef JESTER_GAME_ENGINE_HPP
#define JESTER_GAME_ENGINE_HPP

#include "../players/player.hpp"

#include <string>

namespace jester {

class PlayerOption {
private:
    std::string d_name;
    std::function<std::shared_ptr<Player>(bool)> d_producer;

public:
    PlayerOption(const std::string& name, std::function<std::shared_ptr<Player>(bool)> producer);
    const std::string& name() const;
    std::shared_ptr<Player> produce(bool has_human) const;
};

class GameEngine {
private:
    std::vector<PlayerOption> d_options;

public:
    GameEngine();
    void shell() const;

private:
    void printOptions() const;
};

inline const std::string& PlayerOption::name() const
{
    return d_name;
}

inline std::shared_ptr<Player> PlayerOption::produce(bool has_human) const
{
    return d_producer(has_human);
}

}

#endif
