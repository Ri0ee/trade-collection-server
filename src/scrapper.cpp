#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "data.hpp"
#include "poe-client.hpp"
#include "ninja-client.hpp"
#include "aggregator.hpp"

int main() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::ifstream tokenFile(currentPath / "poetoken");
    if (!tokenFile.good())
        throw std::runtime_error("PoE token not found");

    std::string poeToken;
    tokenFile >> poeToken;

    PoEClient poeClient("www.pathofexile.com", poeToken);
    NinjaClient ninjaClient("poe.ninja");

    std::map<std::string, poeapi::SearchRequest> searchRequests;
    try { 
        std::ifstream searchRequestsFile(currentPath / "requests.json");
        searchRequests = jsoncons::decode_json<std::map<std::string, poeapi::SearchRequest>>(searchRequestsFile);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    if (searchRequests.empty()) {
        std::cout << "No request collection provided\n";
        return 0;
    }

    std::string league = "Standard"; // default
    if (auto leaguesRes = poeClient.getAllLeagues(); leaguesRes.has_value()) {
        auto& leagues = leaguesRes.value();
        auto it = std::remove_if(leagues.begin(), leagues.end(), [](const poeapi::League& league) {
            if (!league.rules.has_value()) 
                return false;
            
            const auto& rules = league.rules.value();
            return std::any_of(rules.begin(), rules.end(), [](const poeapi::League::Rule& rule) {
                return rule.id == "NoParties"; 
            });
        });

        leagues.erase(it, leagues.end());

        std::cout << "Current trade leagues:\n";
        for (auto& league : leagues) {
            std::cout << '\t' << std::quoted(league.id) << '\n';
        }
    }

    double divineOrbChaosEquivalent = 100; // default
    if (auto currencyOverviewResponse = ninjaClient.fetchCurrencyOverview(league); currencyOverviewResponse.has_value()) {
        auto& currencyOverview = currencyOverviewResponse.value(); 
        auto predicate = [](const ninjapi::CurrencyOverviewResponse::Line& line) {
            return line.currencyTypeName == "Divine Orb";
        };
        if (auto divineOrbEntry = std::find_if(currencyOverview.lines.begin(), currencyOverview.lines.end(), predicate); divineOrbEntry != currencyOverview.lines.end()) {
            divineOrbChaosEquivalent = (*divineOrbEntry).chaosEquivalent;
        }
    }

    aggr::updateCurrencyRatio("divine", divineOrbChaosEquivalent);

    Data data;
    for (auto const& [metaName, searchRequest] : searchRequests) {
        if (auto searchResponse = poeClient.search(searchRequest); searchResponse.has_value() && !searchResponse.value().result.empty()) {
            auto& hashes = searchResponse.value();
            if (auto fetchResponse = poeClient.fetch(hashes.id, hashes.result); fetchResponse.has_value()) {
                auto& items = fetchResponse.value();
                auto avgChaosPrice = aggr::averageChaosPrice(items);
                data.items.push_back( Data::Item { 
                    .metaName = metaName, 
                    .averageCostChaos = avgChaosPrice, 
                    .averageCostDivine = avgChaosPrice / aggr::currencyToChaosRatios["divine"] 
                });
            }
        }
        data.update();
    }

    return 0;
}
