#include "mccfr_engine.hpp"

#include "../game/game_state.hpp"
#include "../game/game_view.hpp"
#include "../logging.hpp"

/* #include <cereal/archives/portable_binary.hpp> */
#include <cereal/archives/json.hpp>

#include <fstream>

namespace jester {

constexpr static size_t UPDATES_PER_INTERVAL = 24;

MCCFREngine::MCCFREngine(const std::string& filename)
    : d_filename(filename)
{
    std::ifstream infile(filename);
    if (infile.good()) {
        cereal::JSONInputArchive iarchive(infile);
        iarchive(d_strategy);
    }
}

void MCCFREngine::train()
{
    auto threads = trainingThreads(std::thread::hardware_concurrency());
    threads.push_back(savingThread(std::chrono::seconds(120)));
    for (auto& thr : threads) {
        thr.join();
    }
}

void MCCFREngine::save()
{
    std::lock_guard<std::mutex> lck(d_strategy.mutex());
    if (training_logger != nullptr) {
        training_logger->info("Saving {} information sets.", d_strategy.table().size());
    }
    std::ofstream outfile(d_filename);
    if (outfile.good()) {
        cereal::JSONOutputArchive oarchive(outfile);
        oarchive(d_strategy);
    } else {
        throw std::runtime_error("Unable to save MCCFR table to disk.");
    }
    if (training_logger != nullptr) {
        training_logger->info("Save completed.");
    }
}

std::vector<std::thread> MCCFREngine::trainingThreads(size_t num_threads)
{
    std::vector<std::thread> threads;
    for (size_t t = 0; t < num_threads; t++) {
        threads.push_back(std::thread([this, t]() {
            std::mt19937 rng(std::random_device {}());
            if (training_logger != nullptr) {
                training_logger->info("MCCFR training thread {} started.", t);
            }
            while (true) {
                size_t num_players = 2;
                GameState root(num_players, rng);
                for (size_t tpid = 0; tpid < num_players; tpid++) {
                    d_strategy.train(tpid, root, rng);
                }
            }
        }));
    }
    return threads;
}

std::thread MCCFREngine::savingThread(const std::chrono::milliseconds& interval)
{
    std::thread thr([interval, this]() {
        if (training_logger != nullptr) {
            training_logger->info("MCCFR save thread started.");
        }
        while (true) {
            if (training_logger != nullptr) {
                for (size_t i = 0; i < UPDATES_PER_INTERVAL; i++) {
                    {
                        std::lock_guard<std::mutex> lck(d_strategy.mutex());
                        training_logger->info("{} information sets in memory.",
                            d_strategy.table().size());
                    }
                    std::this_thread::sleep_for(interval / UPDATES_PER_INTERVAL);
                }
            } else {
                std::this_thread::sleep_for(interval);
            }
            save();
        }
    });
    return thr;
}

}
