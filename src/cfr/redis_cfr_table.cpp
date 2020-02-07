#include <cfr/cfr_distribution.hpp>
#include <cfr/cfr_infoset.hpp>
#include <cfr/redis_cfr_table.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>

#include <algorithm>
#include <sstream>

#include <cereal/archives/portable_binary.hpp>

namespace jester {

RedisCFRTable::RedisCFRTable()
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

std::unique_ptr<CFRDistribution> RedisCFRTable::findProfile(const CFRInfoSet& infoset, size_t num_actions)
{
    CFRDistribution* result = nullptr;
    for (size_t idx = 0; idx < num_actions; idx++) {
        std::stringstream iss;
        iss << "jester.profile:";
        {
            cereal::PortableBinaryOutputArchive oarchive(iss);
            oarchive(infoset);
        }
        iss << ":" << idx;
        auto& cmd = d_client.commandSync<std::string>({ "GET", iss.str() });
        if (cmd.ok()) {
            if (result == nullptr) {
                result = new CFRDistribution(num_actions);
            }
            result->add(idx, std::stoi(cmd.reply()));
        }
        cmd.free();
    }
    if (result != nullptr) {
        d_hits++;
        return std::unique_ptr<CFRDistribution>(result);
    } else {
        d_misses++;
        return nullptr;
    }
}

void RedisCFRTable::saveRegret(const CFRInfoSet& infoset,
    const CFRDistribution& distribution)
{
    std::stringstream iss;
    iss << "jester.regrets:";
    std::stringstream ess;
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    {
        cereal::PortableBinaryOutputArchive oarchive(ess);
        oarchive(distribution);
    }
    d_client.command<std::string>({ "SET", iss.str(), ess.str() });
}

void RedisCFRTable::incrementProfile(const CFRInfoSet& infoset, size_t idx, size_t num_actions)
{
    std::stringstream iss;
    iss << "jester.profile:";
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    iss << ":" << idx;
    d_client.command<int>({ "INCR", iss.str() });
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
