#include "cfrm/cfrm_environment.hpp"
#include "constants.hpp"
#include "engine/game_engine.hpp"

#include <cxxopts.hpp>

using namespace jester;

int main(int argc, char** argv)
{
    cxxopts::Options options("jester", "A suite of AI players for the card game Durak.");
    options.add_options()("r,reduced", "Use reduced form of Durak with only 2 ranks");
    options.add_options()("t,train", "Train CFRM (max threads, 30 seconds save interval)");
    options.add_options()("h,help", "Print help message");
    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        std::cout << options.help();
        return EXIT_SUCCESS;
    }

    if (result.count("reduced") > 0) {
        std::cout << "!!! WARNING !!! Using a reduced form of Durak at the moment!" << std::endl;
        std::cout << "!!! WARNING !!! Only ranks 6-7 are being used." << std::endl;
        std::cout << std::endl;
        Constants::instance().MAX_RANK = 7;
    }

    if (result.count("train") > 0) {
        CFRMEnvironment env;
        env.train();
    } else {
        GameEngine engine;
        engine.shell();
    }
}
