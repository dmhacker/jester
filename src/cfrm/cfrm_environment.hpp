#ifndef JESTER_CFRM_ENVIRONMENT_HPP
#define JESTER_CFRM_ENVIRONMENT_HPP

#include "cfrm_table.hpp"

#include <chrono>

namespace jester {

class CFRMEnvironment {
private:
    std::string d_filename;
    CFRMTable d_strategy;

public:
    CFRMEnvironment(const std::string& filename = "cfrm.bin");

    CFRMTable& strategy();

    void train();
    void save();

private:
    std::vector<std::thread> trainingThreads(size_t);
    std::thread savingThread(const std::chrono::milliseconds&);
};

inline CFRMTable& CFRMEnvironment::strategy()
{
    return d_strategy;
}

}

#endif
