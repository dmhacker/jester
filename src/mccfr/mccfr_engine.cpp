#include <game/game_state.hpp>
#include <game/game_view.hpp>
#include <logs/loggers.hpp>
#include <mccfr/mccfr_engine.hpp>

#include <cereal/archives/portable_binary.hpp>

#include <cstdlib>
#include <fstream>

namespace jester {

MCCFREngine::MCCFREngine()
    : d_strategy(d_rdx)
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

MCCFREngine::MCCFREngine(const std::string& url, int port)
    : d_strategy(d_rdx)
{
    connect(url, port);
}

MCCFREngine::~MCCFREngine()
{
    d_rdx.disconnect();
}

void MCCFREngine::train()
{
    auto threads = trainingThreads(std::thread::hardware_concurrency());
    threads.push_back(logThread(std::chrono::seconds(5)));
    for (auto& thr : threads) {
        thr.join();
    }
}

std::thread MCCFREngine::logThread(const std::chrono::milliseconds& delay)
{
    std::thread thr([this, delay]() {
        while (training_logger != nullptr) {
            auto& cmd = d_rdx.commandSync<int>({ "DBSIZE" });
            if (cmd.ok()) {
                training_logger->info("{} information sets in storage.", 
                        cmd.reply());
            }
            cmd.free();
            std::this_thread::sleep_for(delay);
        }
    });
    return thr;
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

}
