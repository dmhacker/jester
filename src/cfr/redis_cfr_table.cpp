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

std::unique_ptr<CFREntry> RedisCFRTable::find(const CFRInfoSet& infoset)
{
    std::stringstream iss;
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    auto& cmd = d_client.commandSync<std::string>({ "GET", iss.str() });
    if (cmd.ok()) {
        std::stringstream ess;
        ess << cmd.reply();
        CFREntry* entry = new CFREntry();
        {
            cereal::PortableBinaryInputArchive iarchive(ess);
            iarchive(*entry);
        }
        cmd.free();
        onFind(true);
        return std::unique_ptr<CFREntry>(entry);
    } else {
        cmd.free();
        onFind(false);
        return nullptr;
    }
}

void RedisCFRTable::save(const CFRInfoSet& infoset, const CFREntry& entry)
{
    std::stringstream iss;
    std::stringstream ess;
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    {
        cereal::PortableBinaryOutputArchive oarchive(ess);
        oarchive(entry);
    }
    d_client.command<std::string>({ "SET", iss.str(), ess.str() },
        [](redox::Command<std::string>& c) {
        });
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
