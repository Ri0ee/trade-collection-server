#pragma once

#include <algorithm>
#include <numeric>
#include <sstream>
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

    std::optional<std::vector<poeapi::League>> getAllLeagues() {
        static std::string url(poeapi::ep::leagues);
        if (auto res = Get(url).value(); res.status == 200) {
            return jsoncons::decode_json<std::vector<poeapi::League>>(res.body);
        }
        return std::nullopt;
    }

    std::optional<poeapi::SearchResponse> search(const poeapi::SearchRequest& searchRequest, const std::string& league = "Standard") {
        std::string requestStr;
        jsoncons::encode_json(json { searchRequest }, requestStr);

        std::string url = fmt::format(poeapi::ep::search, league);
        std::this_thread::sleep_until(searchRateLimit.waitUntil());
        if (auto res = Post(url, requestStr, "application/json").value(); res.status == 200) {
            searchRateLimit = {
                .responseTimepoint = steady_clock::now(),
                .account = res.get_header_value("x-rate-limit-account"),
                .accountState = res.get_header_value("x-rate-limit-account-state"),
                .ip = res.get_header_value("x-rate-limit-ip"),
                .ipState = res.get_header_value("x-rate-limit-ip-state"),
                .policy = res.get_header_value("x-rate-limit-policy"),
                .rules = res.get_header_value("x-rate-limit-rules")
            };

            return jsoncons::decode_json<poeapi::SearchResponse>(res.body);
        }

        return std::nullopt;
    }

    std::optional<poeapi::FetchResponse> fetch(const std::string& id, const std::vector<std::string>& hashes) {
        poeapi::FetchResponse fetchResponse = {};
        auto iter = hashes.begin();
        constexpr uint64_t fetchBatchSize = 10;
        for (int i = 0; i < hashes.size() / fetchBatchSize; i++) {
            std::string hashesCombined = std::accumulate(iter, std::min(iter + fetchBatchSize, hashes.end()), std::string(""), [](const auto& str1, const auto& str2) {
                return (str1.empty() ? str2 : str1 + "," + str2);
            });
            iter += fetchBatchSize;

            std::string url = fmt::format(poeapi::ep::fetch, hashesCombined, id);
            std::cout << fmt::format("Batch {} pending, URL: \"{}\"", i, url);

            std::this_thread::sleep_until(fetchRateLimit.waitUntil());
            if (auto res = Get(url).value(); res.status == 200) {
                std::cout << fmt::format("\tBatch {} obtained\n", i);

                fetchRateLimit = {
                    .responseTimepoint = steady_clock::now(),
                    .account = res.get_header_value("x-rate-limit-account"),
                    .accountState = res.get_header_value("x-rate-limit-account-state"),
                    .ip = res.get_header_value("x-rate-limit-ip"),
                    .ipState = res.get_header_value("x-rate-limit-ip-state"),
                    .policy = res.get_header_value("x-rate-limit-policy"),
                    .rules = res.get_header_value("x-rate-limit-rules")
                };

                auto tempFetchResponse = jsoncons::decode_json<poeapi::FetchResponse>(res.body);
                fetchResponse.result.insert(fetchResponse.result.end(), tempFetchResponse.result.begin(), tempFetchResponse.result.end());
            }
        }

        return fetchResponse;
    }

    struct RateLimit {
        steady_clock::time_point responseTimepoint {};
        std::string account {};
        std::string accountState {};
        std::string ip {};
        std::string ipState {};
        std::string policy {};
        std::string rules {};

        milliseconds timeToWait() {
            milliseconds delay { 0 };
            if (rules.find("Ip") != std::string::npos) {
                std::stringstream stream(ip);
                std::string rule;
                while (std::getline(stream, rule, ',')) {
                    int requests, time, penalty;
                    parseRule(rule, requests, time, penalty); 
                    delay = std::max(delay, milliseconds { (time * 1000) / requests });
                }
            } 

            if (rules.find("Account") != std::string::npos) {
                std::stringstream stream(account);
                std::string rule;
                while (std::getline(stream, rule, ',')) {
                    int requests, time, penalty;
                    parseRule(rule, requests, time, penalty); 
                    delay = std::max(delay, milliseconds { (time * 1000) / requests });
                }
            }

            return delay;
        }

        steady_clock::time_point waitUntil() {
            return responseTimepoint + timeToWait();
        }

        void parseRule(const std::string& rule, int& requests, int& time, int& penalty) {
            char delim;
            std::stringstream ruleStream(rule);
            ruleStream >> requests >> delim >> time >> delim >> penalty;
        }
    } searchRateLimit, fetchRateLimit;
};
