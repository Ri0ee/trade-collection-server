#include <algorithm>
#include <numeric>
#include "aggregator.hpp"

std::map<std::string, double> aggr::currencyToChaosRatios = { {"chaos", 1.0} };

void aggr::updateCurrencyRatio(const std::string& currency, double newVal) {
    aggr::currencyToChaosRatios[currency] = newVal;
}

double aggr::averageChaosPrice(const poeapi::FetchResponse& fetchResponse) {
    uint64_t counter = 0;
    double total = std::accumulate(fetchResponse.result.begin(), fetchResponse.result.end(), 0, [&](double sum, const poeapi::FetchResponse::Result& entry) {
        // Skipping listing when the currency type is unknown (or not translated)
        if (aggr::currencyToChaosRatios.contains(entry.listing.price.currency)) {
            sum += entry.listing.price.amount * aggr::currencyToChaosRatios[entry.listing.price.currency];
            counter++;
        }

        return sum;
    });

    return total / counter;
}
