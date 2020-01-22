#include <cfr/redis_cfr_table.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <utils/logging.hpp>

#include <algorithm>
#include <sstream>

#include <cereal/archives/portable_binary.hpp>

namespace jester {

RedisCFRTable::RedisCFRTable()
{
    if (std::getenv("REDIS_URL") != nullptr) {
        std::string url = std::getenv("REDIS_URL");
        int port = std::stoi(std::getenv("REDIS_PORT"));
        d_client.connect(url, port);
    } else {
        d_client.connect();
    }
    
}

RedisCFRTable::RedisCFRTable(const std::string& url, int port)
{
    d_client.connect(url, port);
}

RedisCFRTable::~RedisCFRTable()
{
    d_client.disconnect();
}

std::unique_ptr<CFREntry> RedisCFRTable::find(const CFRInfoSet& infoset)
{
    std::stringstream iss;
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    auto ftr = d_client.get(iss.str());
    d_client.commit();
    auto reply = ftr.get();
    if (reply.ok() && reply.is_string()) {
        std::stringstream ess;
        ess << reply.as_string();
        CFREntry* entry = new CFREntry();
        {
            cereal::PortableBinaryInputArchive iarchive(ess);
            iarchive(*entry);
        }
        onFind(true);
        return std::unique_ptr<CFREntry>(entry);
    } else {
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
    d_client.set(iss.str(), ess.str());
    d_client.commit();
}

size_t RedisCFRTable::size()
{
    size_t sz = 0;
    auto ftr = d_client.dbsize();
    d_client.commit();
    auto reply = ftr.get();
    if (reply.ok()) {
        sz = reply.as_integer();
    }
    return sz;
}

}
