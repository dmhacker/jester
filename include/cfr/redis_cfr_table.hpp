#ifndef JESTER_REDIS_CFR_TABLE_HPP
#define JESTER_REDIS_CFR_TABLE_HPP

#include "cfr_table.hpp"

#include <cpp_redis/cpp_redis>

namespace jester {

class RedisCFRTable : public CFRTable {
private:
    cpp_redis::client d_client;

public:
    RedisCFRTable();
    RedisCFRTable(const std::string& url, int port);
    ~RedisCFRTable();

    std::unique_ptr<CFREntry> find(const CFRInfoSet&);
    void save(const CFRInfoSet&, const CFREntry&);
    size_t size();
};

}

#endif
