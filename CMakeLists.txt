cmake_minimum_required (VERSION 3.7)
project(jester)

find_package(Threads REQUIRED)
include(cmake/Catch.cmake)
include(CTest)

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
target_link_libraries(jester Threads::Threads)

# Main executable allows user to play the game/train CFRM
add_executable(jrun "src/main.cpp")
target_link_libraries(jrun jester)

# Test executable runs tests on the game logic
file(GLOB test_sources 
    "test/*.cpp"
)
add_executable(jtests ${test_sources})
target_link_libraries(jtests jester)
catch_discover_tests(jtests)
