#ifndef JESTER_MCCFR_ENGINE_HPP
#define JESTER_MCCFR_ENGINE_HPP

#include "mccfr_table.hpp"

#include <chrono>
#include <redox.hpp>
#include <sstream>

namespace jester {

class MCCFREngine {
private:
    MCCFRTable d_strategy;
    redox::Redox d_rdx;

public:
    MCCFREngine();
    MCCFREngine(const std::string& url, int port);
    ~MCCFREngine();

    MCCFRTable& strategy();

    void train();

private:
    void connect(const std::string& url, int port);

    std::thread logThread(const std::chrono::milliseconds& delay);
    std::vector<std::thread> trainingThreads(size_t);
};

inline MCCFRTable& MCCFREngine::strategy()
{
    return d_strategy;
}

inline void MCCFREngine::connect(const std::string& url, int port)
{
    if (!d_rdx.connect(url, port)) {
        std::stringstream ss;
        ss << "Could not connect to Redis at "
           << url << ":" << port
           << ".";
        throw std::runtime_error(ss.str());
    }
    d_rdx.logger_.level(redox::log::Level::Error);
}

}

#endif
