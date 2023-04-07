#pragma once

#include <map>
#include "poe-api.hpp"

namespace aggr {
    extern std::map<std::string, double> currencyToChaosRatios;
    void updateCurrencyRatio(const std::string& currency, double newVal);

    double averageChaosPrice(const poeapi::FetchResponse& fetchResponse);
}