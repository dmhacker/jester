#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include <utils/logging.hpp>

namespace jester {

std::shared_ptr<spdlog::logger> bots_logger = nullptr;
std::shared_ptr<spdlog::logger> training_logger = nullptr;

}
