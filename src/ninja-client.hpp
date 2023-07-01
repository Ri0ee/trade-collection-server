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

    std::optional<ninjapi::CurrencyOverviewResponse> fetchCurrencyOverview(const std::string& league, const std::string& language = "english") {
        std::string url = fmt::format(ninjapi::ep::currencyOverview, league, "Currency", language);
        if (auto res = Get(url).value(); res.status == 200) {
            return jsoncons::decode_json<ninjapi::CurrencyOverviewResponse>(res.body);
        }

        return std::nullopt;
    }
};