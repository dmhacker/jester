#ifndef JESTER_REDIS_CFR_TABLE_HPP
#define JESTER_REDIS_CFR_TABLE_HPP

#include "cfr_table.hpp"

#include <redox.hpp>

namespace jester {

class RedisCFRTable : public CFRTable {
private:
    redox::Redox d_client;

public:
    RedisCFRTable();
    RedisCFRTable(const std::string& url, int port);
    ~RedisCFRTable();

    std::unique_ptr<CFRDistribution> findRegret(const CFRInfoSet&);
    std::unique_ptr<CFRDistribution> findProfile(const CFRInfoSet&, size_t num_actions);
    void saveRegret(const CFRInfoSet&, const CFRDistribution&);
    void incrementProfile(const CFRInfoSet&, size_t idx, size_t num_actions);
    size_t size();

private:
    void connect(const std::string& url, int port);
};

}

#endif
