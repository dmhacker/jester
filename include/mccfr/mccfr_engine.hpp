#ifndef JESTER_MCCFR_ENGINE_HPP
#define JESTER_MCCFR_ENGINE_HPP

#include "mccfr_table.hpp"

#include <chrono>

namespace jester {

class MCCFREngine {
private:
    std::string d_filename;
    MCCFRTable d_strategy;

public:
    MCCFREngine(const std::string& filename = "mccfr.bin");

    MCCFRTable& strategy();

    void train();
    void save();

private:
    std::vector<std::thread> trainingThreads(size_t);
    std::thread savingThread(const std::chrono::milliseconds&);
};

inline MCCFRTable& MCCFREngine::strategy()
{
    return d_strategy;
}

}

#endif
