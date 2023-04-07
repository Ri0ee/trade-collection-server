cmake_minimum_required(VERSION 3.12)
include(FetchContent)
project(trade-collection-server)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
)

FetchContent_Declare(
    jsoncons
    GIT_REPOSITORY https://github.com/danielaparker/jsoncons
    GIT_TAG v0.170.0
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
)

FetchContent_MakeAvailable(cpp-httplib jsoncons fmt)

add_executable(server src/server.cpp src/data.cpp)
add_executable(scrapper src/scrapper.cpp src/data.cpp src/aggregator.cpp)
target_link_libraries(server PRIVATE httplib::httplib jsoncons fmt)
target_link_libraries(scrapper PRIVATE httplib::httplib jsoncons fmt)
