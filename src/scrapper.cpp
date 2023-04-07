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
    PoEClient client("www.pathofexile.com", poeToken);

    poeapi::SearchRequest searchRequest {};
    searchRequest.query.type = "Awakened Added Chaos Damage Support";
    searchRequest.query.status.option = "online";
    searchRequest.query.stats.push_back({ .type = "and", .filters = {}});
    searchRequest.sort.price = "asc";

    aggr::updateCurrencyRatio("divine", 100);
    Data data;
    if (auto searchResponse = client.search(searchRequest); searchResponse.has_value()) {
        auto hashes = searchResponse.value();
        if (auto fetchResponse = client.fetch(hashes.id, hashes.result); fetchResponse.has_value()) {
            auto items = fetchResponse.value();
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
