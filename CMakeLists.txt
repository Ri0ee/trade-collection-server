cmake_minimum_required(VERSION 3.12)
include(FetchContent)
project(trade-collection-server)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
)

find_package(OpenSSL REQUIRED)

set(HTTPLIB_IS_USING_OPENSSL TRUE)
FetchContent_MakeAvailable(cpp-httplib)

add_executable(server src/server.cpp)
target_link_libraries(server OpenSSL::SSL OpenSSL::Crypto httplib::httplib)