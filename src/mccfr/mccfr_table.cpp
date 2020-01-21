#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <logs/formatting.hpp>
#include <logs/loggers.hpp>
#include <mccfr/mccfr_table.hpp>

#include <algorithm>
#include <sstream>

#include <cereal/archives/portable_binary.hpp>

namespace jester {

MCCFRTable::MCCFRTable(redox::Redox& redox)
    : d_rdx(redox)
{
}

Action MCCFRTable::bestAction(const GameView& view, std::mt19937& rng)
{
    MCCFRInfoSet key(view);
    auto& cmd = d_rdx.commandSync<std::string>({ "GET", toString(key) });
    if (cmd.ok()) {
        auto actions = view.nextActions();
        std::sort(actions.begin(), actions.end());
        auto profile = fromString(cmd.reply()).averageProfile();
        if (bots_logger != nullptr) {
            std::stringstream ss;
            ss << profile << " for actions " << actions;
            bots_logger->info("MCCFR has distribution {}.", ss.str());
        }
        cmd.free();
        return sampleAction(actions, profile, rng);
    } else {
        if (bots_logger != nullptr) {
            bots_logger->info("MCCFR could not find a matching information set.");
        }
        const auto& actions = view.nextActions();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, actions.size() - 1);
        cmd.free();
        return actions[dist(rng)];
    }
}

Action MCCFRTable::sampleAction(const std::vector<Action>& actions, const std::vector<float>& profile, std::mt19937& rng)
{
    std::uniform_real_distribution<> dist(0, 1);
    float randf = dist(rng);
    float counter = 0.0f;
    for (size_t idx = 0; idx < actions.size(); idx++) {
        counter += profile[idx];
        if (randf <= counter) {
            return actions[idx];
        }
    }
    std::stringstream ss;
    ss << "Distribution " << profile << " does not sum to 1.";
    throw std::logic_error(ss.str());
}

float MCCFRTable::train(size_t tpid, const GameState& state, std::mt19937& rng)
{
    // Return utility from terminal node; this is the evaluation function
    if (state.finished()) {
        float reward = 0;
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

    MCCFRInfoSet key(view);
    MCCFREntry entry(actions.size());
    auto key_str = toString(key);
    auto& cmd = d_rdx.commandSync<std::string>({ "GET", key_str });
    if (cmd.ok()) {
        entry = fromString(cmd.reply());
    }
    cmd.free();
    auto profile = entry.currentProfile();

    // External sampling: opponents only follow one action
    if (player != tpid) {
        auto action = sampleAction(actions, profile, rng);
        GameState next_state(state);
        next_state.playAction(action);
        float result = train(tpid, next_state, rng);
        return result;
    }

    // External sampling: walk every action for the current player
    std::vector<float> child_util(actions.size());
    float total_util = 0.0f;
    for (size_t i = 0; i < actions.size(); i++) {
        GameState next_state(state);
        next_state.playAction(actions[i]);
        child_util[i] = train(tpid, next_state, rng);
        total_util += profile[i] * child_util[i];
    }
    // Update cumulative counterfactual regrets based off of utilities
    for (size_t i = 0; i < actions.size(); i++) {
        float regret = child_util[i] - total_util;
        entry.addRegret(i, regret);
    }
    entry.addProfile(profile);

    // Save profile back into Redis database
    d_rdx.command<std::string>({ "SET", key_str, toString(entry) },
        [](redox::Command<std::string>& c) {
        });

    return total_util;
}

std::string MCCFRTable::toString(const MCCFRInfoSet& infoset)
{
    std::stringstream ss;
    cereal::PortableBinaryOutputArchive oarchive(ss);
    oarchive(infoset);
    return ss.str();
}

std::string MCCFRTable::toString(const MCCFREntry& entry)
{
    std::stringstream ss;
    cereal::PortableBinaryOutputArchive oarchive(ss);
    oarchive(entry);
    return ss.str();
}

MCCFREntry MCCFRTable::fromString(const std::string& strn)
{
    std::stringstream ss;
    ss << strn;
    MCCFREntry entry;
    cereal::PortableBinaryInputArchive iarchive(ss);
    iarchive(entry);
    return entry;
}

}
