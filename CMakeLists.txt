cmake_minimum_required (VERSION 3.7)
project(jester)

# Find dependencies on the user's system
add_subdirectory(lib/cpp_redis)
find_package(Threads REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CTest)
include(cmake/Catch.cmake)

# Generate coverage data and send it to coveralls
option(COVERALLS "Generate coveralls data" OFF)
if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Main library encapsulates all game and player logic
file(GLOB sources 
    "src/**/*.cpp"
)
list(REMOVE_ITEM sources "src/main.cpp")
add_library(jester ${sources})
target_include_directories(jester PUBLIC "include")
target_include_directories(jester PUBLIC "lib/include")
target_include_directories(jester PUBLIC "lib/cpp_redis/includes")
target_include_directories(jester PUBLIC "lib/cpp_redis/tacopie/includes")
target_link_libraries(jester Threads::Threads)
target_link_libraries(jester cpp_redis)

# Main executable allows user to play the game/train CFRM
add_executable(play "src/main.cpp")
target_link_libraries(play jester)

# Test executable runs tests on the game logic
file(GLOB test_sources 
    "test/*.cpp"
)
add_executable(tests ${test_sources})
target_link_libraries(tests jester)
catch_discover_tests(tests)

if (COVERALLS)
    coveralls_setup("${sources}" ON)
endif()
