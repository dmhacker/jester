#include <cfr/cfr_engine.hpp>
#include <cfr/redis_cfr_table.hpp>
#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <utils/logging.hpp>

#include <cereal/archives/portable_binary.hpp>

#include <cstdlib>
#include <fstream>

namespace jester {

CFREngine::CFREngine()
    : d_table(stda::make_erased<RedisCFRTable>())
{
}

CFREngine::CFREngine(stda::erased_ptr<CFRTable>&& table)
    : d_table(std::move(table))
{
}

void CFREngine::train()
{
    std::vector<std::thread> threads;
    auto tt_count = std::max(1u,
        std::thread::hardware_concurrency());
    for (size_t t = 0; t < tt_count; t++) {
        threads.push_back(std::thread([this, t]() {
            std::mt19937 rng(std::random_device {}());
            if (training_logger != nullptr) {
                training_logger->info("CFR training thread {} started.", t);
            }
            while (true) {
                size_t num_players = 2;
                GameState root(num_players, rng);
                for (size_t tpid = 0; tpid < num_players; tpid++) {
                    train(tpid, root, rng);
                }
            }
        }));
    }
    threads.push_back(std::thread([this]() {
        if (training_logger != nullptr) {
            training_logger->info("CFR log thread.");
        }
        while (training_logger != nullptr) {
            training_logger->info("{} information sets in total.",
                d_table->size());
            training_logger->info("{} hits, {} misses this session.",
                d_table->hits(), d_table->misses());
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    }));
    for (auto& thr : threads) {
        thr.join();
    }
}

int CFREngine::train(size_t tpid, const GameState& state, std::mt19937& rng)
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
        auto entry_lookup = d_table->find(key);
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
        d_table->save(key, entry);
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
    d_table->save(key, entry);

    return child_util[best_idx];
}

Action CFREngine::bestAction(const GameView& view, std::mt19937& rng)
{
    auto entry = d_table->find(CFRInfoSet(view));
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

size_t CFREngine::sampleIndex(const std::vector<float>& profile, std::mt19937& rng) const
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

}
