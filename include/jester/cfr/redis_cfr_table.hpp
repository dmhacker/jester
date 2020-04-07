#ifndef JESTER_REDIS_CFR_TABLE_HPP
#define JESTER_REDIS_CFR_TABLE_HPP

#include "cfr_table.hpp"
#include "cfr_infoset.hpp"

#include <redox.hpp>
#include <lrucache.hpp>

#include <mutex>

namespace jester {

class RedisCFRTable : public CFRTable {
private:
    redox::Redox d_client;
    cache::lru_cache<CFRInfoSet, CFRDistribution> d_rcache;
    cache::lru_cache<CFRInfoSet, CFRDistribution> d_pcache;
    std::mutex d_rmtx;
    std::mutex d_pmtx;

public:
    RedisCFRTable();
    RedisCFRTable(const std::string& url, int port);
    ~RedisCFRTable();

    std::unique_ptr<CFRDistribution> findRegret(const CFRInfoSet&);
    std::unique_ptr<CFRDistribution> findProfile(const CFRInfoSet&);
    void saveRegret(const CFRInfoSet&, const CFRDistribution&);
    void saveProfile(const CFRInfoSet&, const CFRDistribution&);
    size_t size();

private:
    void connect(const std::string& url, int port);
};

}

#endif
