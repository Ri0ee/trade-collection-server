#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>

#include "jsoncons/json.hpp"

namespace poeapi {
    namespace ep {
        constexpr std::string_view leagues { "/api/leagues" };
        constexpr std::string_view singleLeague { "/api/leagues/{}" };

        constexpr std::string_view tradeLeagues { "/api/trade/data/leagues" };
        constexpr std::string_view changeIds { "/api/trade/data/change-ids" };
        constexpr std::string_view items { "/api/trade/data/items" };
        constexpr std::string_view stats { "/api/trade/data/stats" };

        constexpr std::string_view search { "/api/trade/search/{}" };
        constexpr std::string_view fetch { "/api/trade/fetch/{}?query={}" };
    }

    struct League {
        struct Rule {
            std::string id;
        };
        std::optional<std::vector<Rule>> rules;
        std::string description;
        std::string id;

        // std::string realm;
        // std::string registerAt;
        // std::string startAt;
        // std::string url;
        // std::optional<bool> delveEvent;
    };

    struct SearchRequest {
        struct Query {
            struct Filters {
                struct TradeFilters {
                    bool disabled = false;
                    struct Filters {
                        struct Price {
                            std::optional<std::string> option;
                            std::optional<double> min;
                            std::optional<double> max;
                        };
                        std::optional<Price> price;

                        struct Account {
                            std::string input;
                        };
                        std::optional<Account> account;

                        struct Indexed {
                            std::string option;
                        };
                        std::optional<Indexed> indexed;
                    } filters;     
                } trade_filters;
            };
            std::optional<Filters> filters;

            std::optional<std::string> name;
            std::optional<std::string> type;

            struct Stat {
                std::optional<bool> disabled;
                std::string type;

                struct Filter {
                    std::optional<bool> disabled;
                    std::string id;
                    struct Value {
                        std::optional<double> min;
                        std::optional<double> max;
                    } value;
                };

                std::vector<Filter> filters;
            };

            std::vector<Stat> stats;

            struct Status {
                std::string option;
            } status;
        } query;

        struct Sort {
            std::string price;
        } sort;
    };

    struct SearchResponse {
        int64_t complexity;
        std::string id;
        std::vector<std::string> result;
        int64_t total;
    };

    struct FetchResponse {
        struct Result {
            std::string id;

            struct Item {
                std::string baseType;
                std::string typeLine;
                std::string league; 
                std::string name;
                std::string icon;
                std::string id;

                std::optional<bool> corrupted;
                std::optional<bool> identified;
                std::optional<bool> verified;
                std::optional<int64_t> frameType;
                std::optional<int64_t> ilvl;
                int64_t w;
                int64_t h;

                struct Extended {
                    std::optional<int64_t> base_defence_percentile;
                    std::string text;
                    
                    struct Mods {
                        struct Explicit {
                            std::string name;
                            std::string tier;
                            int64_t level;

                            struct Magnitudes {
                                std::string hash;
                                std::variant<int64_t, double> min;
                                std::variant<int64_t, double> max;
                            };
                            std::variant<std::vector<Magnitudes>, jsoncons::null_type> magnitudes;
                        };
                        std::optional<std::vector<Explicit>> _explicit;

                        struct Implicit {
                            std::string name;
                            std::string tier;
                            int64_t level;

                            struct Magnitudes {
                                std::string hash;
                                std::variant<int64_t, double> min;
                                std::variant<int64_t, double> max;
                            };
                            std::variant<std::vector<Magnitudes>, jsoncons::null_type> magnitudes;
                        };
                        std::optional<std::vector<Implicit>> implicit;
                    };
                    std::optional<Mods> mods;
                } extended;

                struct Sockets {
                    std::string attr;
                    std::string sColour;
                    int64_t group;
                };
                std::optional<std::vector<Sockets>> sockets;
            } item;

            struct Listing {
                struct Account {
                    std::string language;
                    std::string lastCharacterName;
                    std::string name;
                    std::string realm;

                    struct Online {
                        std::string league;
                        std::optional<std::string> status;
                    };
                    std::variant<Online, jsoncons::null_type> online;
                } account;

                struct Price {
                    double amount;
                    std::string currency;
                    std::string type;
                } price;

                struct Stash {
                    std::string name;
                    int64_t x;
                    int64_t y;
                } stash;

                std::string indexed;
                std::string method;
                std::string whisper;
                std::string whisper_token;
                std::optional<std::string> thread_id;
                std::optional<std::string> thread_locale;
            } listing;
        };

        std::vector<Result> result;
    };
}

JSONCONS_N_MEMBER_TRAITS(poeapi::League::Rule, 0, id);
JSONCONS_N_MEMBER_TRAITS(poeapi::League, 0, description, id, rules);

JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters::TradeFilters::Filters::Price, 0, option, min, max)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters::TradeFilters::Filters::Account, 0, input)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters::TradeFilters::Filters::Indexed, 0, option)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters::TradeFilters::Filters, 0, price, account, indexed)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters::TradeFilters, 0, disabled, filters)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Stat::Filter::Value, 0, min, max)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Stat::Filter, 0, id, value, disabled)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Filters, 0, trade_filters)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Stat, 0, type, filters, disabled)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query::Status, 0, option)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Query, 0, name, type, stats, status, filters)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest::Sort, 0, price)
JSONCONS_N_MEMBER_TRAITS(poeapi::SearchRequest, 0, query, sort)

JSONCONS_N_MEMBER_TRAITS(poeapi::SearchResponse, 0, complexity, id, result, total)

JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Extended::Mods::Implicit::Magnitudes, 0, hash, min, max)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Extended::Mods::Implicit, 0, name, tier, level, magnitudes)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Extended::Mods::Explicit::Magnitudes, 0, hash, min, max)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Extended::Mods::Explicit, 0, name, tier, level, magnitudes)
JSONCONS_N_MEMBER_NAME_TRAITS(poeapi::FetchResponse::Result::Item::Extended::Mods, 0, (_explicit, "explicit"), (implicit, "implicit"))
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Extended, 0, text, base_defence_percentile, mods)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item::Sockets, 0, attr, group, sColour)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Item, 0, baseType, typeLine, league, name, icon, id, w, h, extended, corrupted, identified, verified, frameType, ilvl, sockets)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Listing::Account::Online, 0, league, status)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Listing::Account, 0, language, lastCharacterName, name, online, realm)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Listing::Price, 0, amount, currency, type)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Listing::Stash, 0, name, x, y)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result::Listing, 0, account, price, stash, indexed, method, whisper, whisper_token, thread_id, thread_locale)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse::Result, 0, id, item, listing)
JSONCONS_N_MEMBER_TRAITS(poeapi::FetchResponse, 0, result)