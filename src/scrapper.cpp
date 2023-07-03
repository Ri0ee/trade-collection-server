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

    poeapi::SearchRequest searchRequest {};
    searchRequest.query.type = "Awakened Added Chaos Damage Support";
    searchRequest.query.status.option = "online";
    searchRequest.query.stats.push_back({ .type = "and", .filters = {}});
    searchRequest.sort.price = "asc";

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

        for (auto& league : leagues) {
            std::cout << league.id << '\n';
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
    if (auto searchResponse = poeClient.search(searchRequest); searchResponse.has_value() && !searchResponse.value().result.empty()) {
        auto& hashes = searchResponse.value();
        if (auto fetchResponse = poeClient.fetch(hashes.id, hashes.result); fetchResponse.has_value()) {
            auto& items = fetchResponse.value();
            auto avgChaosPrice = aggr::averageChaosPrice(items);
            data.items.push_back( Data::Item { 
                .metaName = "Awakened Added Chaos Damage Support - all", 
                .averageCostChaos = avgChaosPrice, 
                .averageCostDivine = avgChaosPrice / aggr::currencyToChaosRatios["divine"] 
            });
        } else {
            throw std::runtime_error("Fetch returned no value");
        }
    } else {
        throw std::runtime_error("Search returned no value");
    }

    data.update();
    
    return 0;
}
