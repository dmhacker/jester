#include <cfr/cfr_table.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <utils/logging.hpp>

#include <algorithm>
#include <sstream>

#include <cereal/archives/portable_binary.hpp>

namespace jester {

CFRTable::CFRTable(redox::Redox& redox)
    : d_rdx(redox)
{
}

Action CFRTable::bestAction(const GameView& view, std::mt19937& rng)
{
    auto entry = findEntry(CFRInfoSet(view));
    if (entry != nullptr) {
        auto actions = view.nextActions();
        std::sort(actions.begin(), actions.end());
        auto profile = entry->averageProfile();
        if (bots_logger != nullptr) {
            std::stringstream ss;
            ss << profile << " for actions " << actions;
            bots_logger->info("CFR has distribution {}.", ss.str());
        }
        return actions[sampleIndex(profile, rng)];
    } else {
        if (bots_logger != nullptr) {
            bots_logger->info("CFR could not find a matching information set.");
        }
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        return actions[dist(rng)];
    }
}

int CFRTable::train(size_t tpid, const GameState& state, std::mt19937& rng)
{
    // Return utility from terminal node; this is the evaluation function
    if (state.finished()) {
        int reward = 0;
        for (size_t i = 0; i < state.playerCount(); i++) {
            auto pid = state.winOrder()[i];
            if (pid == tpid) {
                if (i < state.playerCount() - 1) {
                    reward = state.playerCount() - 1 - i;
                } else {
                    reward = state.playerCount() * (state.playerCount() - 1) / 2;
                    reward *= -1;
                }
                break;
            }
        }
        return reward;
    }

    // Skip over information sets where only one action is possible
    auto player = state.currentPlayerId();
    auto actions = state.nextActions();
    auto view = state.currentPlayerView();
    if (actions.size() == 1) {
        auto action = actions[0];
        GameState next_state(state);
        next_state.playAction(action);
        return train(tpid, next_state, rng);
    }
    std::sort(actions.begin(), actions.end());

    CFRInfoSet key(view);
    CFREntry entry(actions.size());
    {
        auto entry_lookup = findEntry(key);
        if (entry_lookup != nullptr) {
            entry = *entry_lookup;
        }
    }
    auto profile = entry.currentProfile();

    // Sample best action given the current profile
    auto best_idx = sampleIndex(profile, rng);

    // Opponents only follow the best action
    if (player != tpid) {
        GameState next_state(state);
        next_state.playAction(actions[best_idx]);
        entry.addUtility(best_idx, 1);
        saveEntry(key, entry);
        return train(tpid, next_state, rng);
    }

    // Walk every action for the current player
    std::vector<int> child_util(actions.size());
    for (size_t i = 0; i < actions.size(); i++) {
        GameState next_state(state);
        next_state.playAction(actions[i]);
        child_util[i] = train(tpid, next_state, rng);
    }
    for (size_t i = 0; i < actions.size(); i++) {
        entry.addRegret(i, child_util[i] - child_util[best_idx]);
    }
    saveEntry(key, entry);

    return child_util[best_idx];
}

size_t CFRTable::sampleIndex(const std::vector<float>& profile, std::mt19937& rng) const
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    for (size_t idx = 0; idx < profile.size(); idx++) {
        counter += profile[idx];
        if (randf <= counter) {
            return idx;
        }
    }
    std::stringstream ss;
    ss << "Distribution " << profile << " does not sum to 1.";
    throw std::logic_error(ss.str());
}

std::unique_ptr<CFREntry> CFRTable::findEntry(const CFRInfoSet& infoset)
{
    std::stringstream iss;
    {
        cereal::PortableBinaryOutputArchive oarchive(iss);
        oarchive(infoset);
    }
    auto& cmd = d_rdx.commandSync<std::string>({ "GET", iss.str() });
    if (cmd.ok()) {
        std::stringstream ess;
        ess << cmd.reply();
        CFREntry* entry = new CFREntry();
        {
            cereal::PortableBinaryInputArchive iarchive(ess);
            iarchive(*entry);
        }
        cmd.free();
        return std::unique_ptr<CFREntry>(entry);
    } else {
        cmd.free();
        return nullptr;
    }
}

void CFRTable::saveEntry(const CFRInfoSet& infoset, const CFREntry& entry)
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
    d_rdx.command<std::string>({ "SET", iss.str(), ess.str() },
        [](redox::Command<std::string>& c) {
        });
}

}
