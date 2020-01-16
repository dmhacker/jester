#include "cfrm/cfrm_environment.hpp"
#include "constants.hpp"
#include "engine/game_engine.hpp"
#include "logging.hpp"

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace jester {

std::shared_ptr<spdlog::logger> bots_logger = spdlog::basic_logger_mt("bots", "logs/bots.log");
std::shared_ptr<spdlog::logger> training_logger = spdlog::stdout_color_mt("training");

}

using namespace jester;

int main(int argc, char** argv)
{
    cxxopts::Options options("jester", "A suite of AI players for the card game Durak.");
    options.add_options()("r,reduced", "Use an extremely reduced form of Durak for testing purposes");
    options.add_options()("t,train", "Train CFRM (max threads, 30 seconds save interval)");
    options.add_options()("h,help", "Print help message");
    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        std::cout << options.help();
        return EXIT_SUCCESS;
    }

    if (result.count("reduced") > 0) {
        Constants::instance().MAX_RANK = 7;
    }

    bots_logger->set_level(spdlog::level::info);
    bots_logger->flush_on(spdlog::level::info); 
    training_logger->set_level(spdlog::level::info);
    training_logger->flush_on(spdlog::level::info); 

    if (result.count("train") > 0) {
        CFRMEnvironment env;
        env.train();
    } else {
        GameEngine engine;
        engine.shell();
    }
}
