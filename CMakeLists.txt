cmake_minimum_required (VERSION 3.7)
project(jester)

find_package(Threads REQUIRED)
include(cmake/Catch.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB sources 
    "src/**/*.cpp"
)
add_executable(jester "src/main.cpp" ${sources})
target_include_directories(jester PRIVATE "include")
target_link_libraries(jester Threads::Threads)

file(GLOB test_sources 
    "test/*.cpp"
)
add_executable(tests "test/main.cpp" ${test_sources})
target_include_directories(tests PRIVATE "include")
target_include_directories(tests PRIVATE "test_include")
target_link_libraries(tests Threads::Threads)
include(CTest)
catch_discover_tests(tests)
