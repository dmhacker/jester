#include "cfrm_environment.hpp"

#include "../rules/game.hpp"
#include "../rules/game_view.hpp"

#include <cereal/archives/portable_binary.hpp>

#include <fstream>

namespace jester {

CFRMEnvironment::CFRMEnvironment(const std::string& filename)
    : d_filename(filename)
{
    std::ifstream infile(filename);
    if (infile.good()) {
        cereal::PortableBinaryInputArchive iarchive(infile);
        iarchive(d_cfrm);
    }
}

void CFRMEnvironment::train()
{
    auto threads = trainingThreads(std::thread::hardware_concurrency());
    threads.push_back(savingThread(std::chrono::seconds(30)));
    for (auto& thr : threads) {
        thr.join();
    }
}

void CFRMEnvironment::save()
{
    std::lock_guard<std::mutex> lck(d_cfrm.mutex());
    std::cout << std::endl << "Saving in progress." << std::endl;
    std::ofstream outfile(d_filename);
    if (outfile.good()) {
        cereal::PortableBinaryOutputArchive oarchive(outfile);
        oarchive(d_cfrm);
    } else {
        throw std::runtime_error("Unable to save CFRM to disk.");
    }
    std::cout << "Save completed." << std::endl;
}

std::vector<std::thread> CFRMEnvironment::trainingThreads(size_t num_threads)
{
    std::vector<std::thread> threads;
    for (size_t t = 0; t < num_threads; t++) {
        threads.push_back(std::thread([&]() {
            std::mt19937 rng(std::random_device {}());
            while (true) {
                size_t num_players = 2;
                std::vector<std::shared_ptr<Player>> players(num_players);
                Game game(players);
                for (size_t tpid = 0; tpid < num_players; tpid++) {
                    d_cfrm.train(tpid, game, rng);
                }
            }
        }));
    }
    return threads;
}

std::thread CFRMEnvironment::savingThread(const std::chrono::milliseconds& interval)
{
    std::thread thr([&interval, this]() {
        auto start_timestamp = std::chrono::system_clock::now();
        while (true) {
            auto end_timestamp = std::chrono::system_clock::now();
            if (end_timestamp - start_timestamp > interval) {
                save();
                start_timestamp = std::chrono::system_clock::now();
            }
        }
    });
    return thr;
}
}
