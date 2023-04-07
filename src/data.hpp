#pragma once

#include "jsoncons/json.hpp"

using json = jsoncons::json;

struct Data {
    void read();
    void update();
    std::string serve() const;

    std::string version = "0";
    std::string date = "";

    struct Item {
        std::string metaName;
        double averageCostChaos;
        double averageCostDivine;
    };
    std::vector<Item> items;
};

JSONCONS_N_MEMBER_TRAITS(Data::Item, 0, metaName, averageCostChaos, averageCostDivine)
JSONCONS_N_MEMBER_TRAITS(Data, 0, version, date, items)