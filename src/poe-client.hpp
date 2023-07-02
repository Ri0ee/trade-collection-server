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

#include "poe-api.hpp"

struct PoEClient : httplib::SSLClient {
    PoEClient(const std::string& host, const std::string& poeToken) : httplib::SSLClient(host) {
        set_default_headers({
            { "cookie", fmt::format("POESESSID={}", poeToken)}
        });
    }

    std::optional<poeapi::LeaguesResponse> getLeagues() {
        
        return std::nullopt;
    }

    std::optional<poeapi::SearchResponse> search(const poeapi::SearchRequest& searchRequest) {
        std::string requestStr;
        jsoncons::encode_json(json { searchRequest }, requestStr);

        std::string url = fmt::format(poeapi::ep::search, "Standard");
        if (auto res = Post(url, requestStr, "application/json").value(); res.status == 200) {
            searchRateLimit = {
                .account = res.get_header_value("x-rate-limit-account"),
                .accountState = res.get_header_value("x-rate-limit-account-state"),
                .ip = res.get_header_value("x-rate-limit-ip"),
                .ipState = res.get_header_value("x-rate-limit-ip-state"),
                .policy = res.get_header_value("x-rate-limit-policy"),
                .rules = res.get_header_value("x-rate-limit-rules")
            };

            return jsoncons::decode_json<poeapi::SearchResponse>(res.body);
        }
        else {
            throw std::runtime_error(fmt::format("POST response code is {}", res.status));
        }

        return std::nullopt;
    }

    std::optional<poeapi::FetchResponse> fetch(const std::string& id, const std::vector<std::string>& hashes) {
        poeapi::FetchResponse fetchResponse = {};
        auto iter = hashes.begin();
        constexpr uint64_t fetchBatchSize = 10;
        for (int i = 0; i < hashes.size() / fetchBatchSize - 8; i++) {
            std::string hashesCombined = std::accumulate(iter, std::min(iter + fetchBatchSize, hashes.end()), std::string(""), [](const auto& str1, const auto& str2) {
                return (str1.empty() ? str2 : str1 + "," + str2);
            });
            iter += fetchBatchSize;

            std::string url = fmt::format(poeapi::ep::fetch, hashesCombined, id);
            std::cout << fmt::format("Batch {} pending, URL: {}\n", i, url);
            if (auto res = Get(url).value(); res.status == 200) {
                std::cout << fmt::format("Batch {} obtained\n", i);

                fetchRateLimit = {
                    .account = res.get_header_value("x-rate-limit-account"),
                    .accountState = res.get_header_value("x-rate-limit-account-state"),
                    .ip = res.get_header_value("x-rate-limit-ip"),
                    .ipState = res.get_header_value("x-rate-limit-ip-state"),
                    .policy = res.get_header_value("x-rate-limit-policy"),
                    .rules = res.get_header_value("x-rate-limit-rules")
                };

                std::cout << jsoncons::pretty_print(json::parse(res.body)) << '\n';
                std::cout << fetchRateLimit.accountState << " " << fetchRateLimit.ipState << '\n';

                auto tempFetchResponse = jsoncons::decode_json<poeapi::FetchResponse>(res.body);
                fetchResponse.result.insert(fetchResponse.result.end(), tempFetchResponse.result.begin(), tempFetchResponse.result.end());
            }

            std::this_thread::sleep_for(10s);
        }

        return fetchResponse;
    }

    struct RateLimit {
        std::string account {};
        std::string accountState {};
        std::string ip {};
        std::string ipState {};
        std::string policy {};
        std::string rules {};
    } searchRateLimit, fetchRateLimit;
};
