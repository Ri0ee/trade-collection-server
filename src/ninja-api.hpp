#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>

#include "jsoncons/json.hpp"

namespace ninjapi {
    namespace ep {
        constexpr std::string_view currencyOverview = "/api/data/currencyoverview?league={}&type={}&language={}";
    }

    struct CurrencyOverviewResponse {
        struct CurrencyDetail {
            int64_t id;
            std::string icon;
            std::string name;
            std::string tradeId;
        };
        std::vector<CurrencyDetail> currentcyDetails;

        struct Language {
            std::string name;
        } language;

        struct Line {
            double chaosEquivalent;
            std::string currencyTypeName;
            std::string detailsId;
            
            struct LowConfidencePaySparkLine {
                double totalChange;
                std::vector<std::optional<double>> data;
            } lowConfidencePaySparkLine;

            struct LowConfidenceReceiveSparkLine {
                double totalChange;
                std::vector<std::optional<double>> data;
            } lowConfidenceReceiveSparkLine;

            struct Pay {
                int64_t count;
                int64_t data_point_count;
                int64_t get_currency_id;
                int64_t id;
                int64_t league_id;
                int64_t listing_count;
                int64_t pay_currency_id;
                std::string sample_time_utc;
                bool includes_secondary;
                double value;
            } pay;

            struct PaySparkLine {
                double totalChange;
                std::vector<std::optional<double>> data;
            } paySparkLine;

            struct Receive {
                int64_t count;
                int64_t data_point_count;
                int64_t get_currency_id;
                int64_t id;
                int64_t league_id;
                int64_t listing_count;
                int64_t pay_currency_id;
                std::string sample_time_utc;
                bool includes_secondary;
                double value;
            } receive;

            struct ReceiveSparkLine {
                double totalChange;
                std::vector<std::optional<double>> data;
            } receiveSparkLine;
        };
        std::vector<Line> lines;
    };
}

JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::CurrencyDetail, 0, id, icon, name, tradeId)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Language, 0, name)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::LowConfidencePaySparkLine, 0, totalChange, data)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::LowConfidenceReceiveSparkLine, 0, totalChange, data)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::Pay, 0, count, data_point_count, get_currency_id, id, league_id, listing_count, pay_currency_id, sample_time_utc, includes_secondary, value)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::PaySparkLine, 0, totalChange, data)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::Receive, 0, count, data_point_count, get_currency_id, id, league_id, listing_count, pay_currency_id, sample_time_utc, includes_secondary, value)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line::ReceiveSparkLine, 0, totalChange, data)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse::Line, 0, chaosEquivalent, currencyTypeName, detailsId, lowConfidencePaySparkLine, lowConfidenceReceiveSparkLine, pay, paySparkLine, receive, receiveSparkLine)
JSONCONS_N_MEMBER_TRAITS(ninjapi::CurrencyOverviewResponse, 0, currentcyDetails, language, lines)