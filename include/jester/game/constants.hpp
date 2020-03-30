#ifndef JESTER_CONSTANTS_HPP
#define JESTER_CONSTANTS_HPP

#include <memory>
#include <random>

namespace jester {

class Constants {
public:
    static Constants& instance()
    {
        static Constants instance;
        return instance;
    }

private:
    Constants() {}


public:
    Constants(Constants const&) = delete;
    void operator=(Constants const&) = delete;

    size_t MIN_RANK = 6;
    size_t MAX_RANK = 14;
    size_t HAND_SIZE = 6;
    size_t MIN_PLAYERS = 2;
    size_t MAX_PLAYERS = 6;
};

}

#endif
