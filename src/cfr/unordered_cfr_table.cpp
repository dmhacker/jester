#include <cfr/unordered_cfr_table.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>

#include <algorithm>

namespace jester {

std::unique_ptr<CFREntry> UnorderedCFRTable::find(const CFRInfoSet& infoset)
{
    std::lock_guard<std::mutex> lck(d_mtx);
    auto it = d_map.find(infoset); 
    if (it != d_map.end()) {
        onFind(true);
        return std::unique_ptr<CFREntry>(new CFREntry(it->second));
    }
    else {
        onFind(false);
        return nullptr;
    }
}

void UnorderedCFRTable::save(const CFRInfoSet& infoset, const CFREntry& entry)
{
    std::lock_guard<std::mutex> lck(d_mtx);
    d_map[infoset] = entry; 
}

size_t UnorderedCFRTable::size()
{
    std::lock_guard<std::mutex> lck(d_mtx);
    return d_map.size(); 
}

}
