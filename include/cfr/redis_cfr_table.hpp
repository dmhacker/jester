#ifndef JESTER_REDIS_CFR_TABLE_HPP
#define JESTER_REDIS_CFR_TABLE_HPP

#include "cfr_table.hpp"

#include <redox.hpp>

namespace jester {

class RedisCFRTable : public CFRTable {
private:
    redox::Redox d_rdx;

public:
    RedisCFRTable();
    RedisCFRTable(const std::string& url, int port);
    ~RedisCFRTable();

    std::unique_ptr<CFREntry> find(const CFRInfoSet&);
    void save(const CFRInfoSet&, const CFREntry&);
    size_t size();

private:
    void connect(const std::string& url, int port);
};

}

#endif
