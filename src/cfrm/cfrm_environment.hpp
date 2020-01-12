#ifndef JESTER_CFRM_ENVIRONMENT_HPP
#define JESTER_CFRM_ENVIRONMENT_HPP

#include "cfrm_tables.hpp"

#include <chrono>

namespace jester {

class CFRMEnvironment {
private:
    std::string d_filename;
    CFRMTables d_cfrm;

public:
    CFRMEnvironment(const std::string& filename = "cfrm.bin");

    CFRMTables& cfrm();

    void train();
    void save();

private:
    std::vector<std::thread> trainingThreads(size_t);
    std::thread savingThread(const std::chrono::milliseconds&);
};

inline CFRMTables& CFRMEnvironment::cfrm()
{
    return d_cfrm;
}

}

#endif
