#pragma once

#include "jsoncons/json.hpp"

using json = jsoncons::json;

struct Data {
    void read();
    void update();
    void restore();
    std::string serve() const;

    std::string version = "0";
    std::string date = "";

    struct Item {
        double averageCostChaos;
        double averageCostDivine;
    };
    std::map<std::string, Item> items;
};

JSONCONS_N_MEMBER_TRAITS(Data::Item, 0, averageCostChaos, averageCostDivine)
JSONCONS_N_MEMBER_TRAITS(Data, 0, version, date, items)