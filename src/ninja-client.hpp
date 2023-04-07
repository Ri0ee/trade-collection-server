#pragma once

#include <algorithm>
#include <numeric>
#include <chrono>
using namespace std::chrono;

#include <thread>
#include <fmt/core.h>

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include "httplib.h"

#include "ninja-api.hpp"

struct NinjaClient : httplib::SSLClient {
    NinjaClient(const std::string& host) : httplib::SSLClient(host) {}
};