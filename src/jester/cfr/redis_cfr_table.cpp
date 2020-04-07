#include <jester/cfr/cfr_distribution.hpp>
#include <jester/cfr/cfr_infoset.hpp>
#include <jester/cfr/redis_cfr_table.hpp>
#include <jester/game/game_state.hpp>
#include <jester/game/game_view.hpp>

#include <cereal/archives/portable_binary.hpp>

#include <algorithm>
#include <sstream>

namespace jester {

constexpr int MAX_CACHE_SIZE = 10000;

RedisCFRTable::RedisCFRTable()
    : d_rcache(MAX_CACHE_SIZE)
    , d_pcache(MAX_CACHE_SIZE)
{
    std::string url;
    if (std::getenv("REDIS_URL") != nullptr) {
        url = std::getenv("REDIS_URL");
    } else {
        url = "localhost";
    }
    int port;
    if (std::getenv("REDIS_PORT") != nullptr) {
        port = std::stoi(std::getenv("REDIS_PORT"));
    } else {
        port = 6379;
    }
    connect(url, port);
}

RedisCFRTable::RedisCFRTable(const std::string& url, int port)
    : d_rcache(MAX_CACHE_SIZE)
    , d_pcache(MAX_CACHE_SIZE)
{
    connect(url, port);
}

RedisCFRTable::~RedisCFRTable()
{
    d_client.disconnect();
}

void RedisCFRTable::connect(const std::string& url, int port)
{
    if (!d_client.connect(url, port)) {
        std::stringstream ss;
        ss << "Could not connect to Redis at "
           << url << ":" << port
           << ".";
        throw std::runtime_error(ss.str());
    }
    d_client.logger_.level(redox::log::Level::Error);
}

std::unique_ptr<CFRDistribution> RedisCFRTable::findRegret(const CFRInfoSet& infoset)
{
    {
        std::lock_guard<std::mutex> lck(d_rmtx);
        if (d_rcache.exists(infoset)) {
            return std::unique_ptr<CFRDistribution>(
                new CFRDistribution(d_rcache.get(infoset)));
        }
    }
    std::stringstream iss;
    iss << "jester.regrets:";
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    auto& cmd = d_client.commandSync<std::string>({ "GET", iss.str() });
    if (cmd.ok()) {
        std::stringstream ess;
        ess << cmd.reply();
        CFRDistribution* result = new CFRDistribution();
        {
            cereal::PortableBinaryInputArchive iarchive(ess);
            iarchive(*result);
        }
        cmd.free();
        d_hits++;
        return std::unique_ptr<CFRDistribution>(result);
    } else {
        cmd.free();
        d_misses++;
        return nullptr;
    }
}

std::unique_ptr<CFRDistribution> RedisCFRTable::findProfile(const CFRInfoSet& infoset)
{
    {
        std::lock_guard<std::mutex> lck(d_pmtx);
        if (d_pcache.exists(infoset)) {
            return std::unique_ptr<CFRDistribution>(
                new CFRDistribution(d_pcache.get(infoset)));
        }
    }
    std::stringstream iss;
    iss << "jester.profile:";
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    auto& cmd = d_client.commandSync<std::string>({ "GET", iss.str() });
    if (cmd.ok()) {
        std::stringstream ess;
        ess << cmd.reply();
        CFRDistribution* result = new CFRDistribution();
        {
            cereal::PortableBinaryInputArchive iarchive(ess);
            iarchive(*result);
        }
        cmd.free();
        d_hits++;
        return std::unique_ptr<CFRDistribution>(result);
    } else {
        cmd.free();
        d_misses++;
        return nullptr;
    }
}

void RedisCFRTable::saveRegret(const CFRInfoSet& infoset,
    const CFRDistribution& distribution)
{
    std::stringstream iss;
    iss << "jester.regrets:";
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    std::stringstream ess;
    {
        cereal::PortableBinaryOutputArchive oarchive(ess);
        oarchive(distribution);
    }
    d_client.command<std::string>({ "SET", iss.str(), ess.str() });
    {
        std::lock_guard<std::mutex> lck(d_rmtx);
        d_rcache.put(infoset, distribution);
    }
}

void RedisCFRTable::saveProfile(const CFRInfoSet& infoset,
    const CFRDistribution& distribution)
{
    std::stringstream iss;
    iss << "jester.profile:";
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    std::stringstream ess;
    {
        cereal::PortableBinaryOutputArchive oarchive(ess);
        oarchive(distribution);
    }
    d_client.command<std::string>({ "SET", iss.str(), ess.str() });
    {
        std::lock_guard<std::mutex> lck(d_pmtx);
        d_pcache.put(infoset, distribution);
    }
}

size_t RedisCFRTable::size()
{
    size_t sz = 0;
    auto& cmd = d_client.commandSync<int>({ "DBSIZE" });
    if (cmd.ok()) {
        sz = cmd.reply();
    }
    cmd.free();
    return sz;
}

}
