#include "data.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>

void Data::read() {
    std::filesystem::path currentPath { std::filesystem::current_path() };

    std::ifstream versionFile(currentPath / "version");
    if (!versionFile.good())
        throw std::runtime_error("Unable to read version file");

    std::stringstream fileVersionBuffer;
    fileVersionBuffer << versionFile.rdbuf();

    if (version == fileVersionBuffer.str()) {
        throw std::runtime_error("No new data available");
    }

    std::ifstream dataFile(currentPath / "data.json");
    if (!dataFile.good())
        throw std::runtime_error("Unable to read data file");
    
    *this = jsoncons::decode_json<Data>(dataFile);

    version = fileVersionBuffer.str();
}

std::string Data::serve() const {
    std::string res;
    jsoncons::encode_json(*this, res);
    return res;
}

void Data::update() {
    std::filesystem::path currentPath { std::filesystem::current_path() };

    std::ofstream versionFile(currentPath / "version");
    if (!versionFile.good())
        throw std::runtime_error("Unable to write version file");

    version = std::to_string(std::stoi(version) + 1);
    versionFile << version;

    std::ofstream dataFile(currentPath / "data.json");
    if (!dataFile.good())
        throw std::runtime_error("Unable to write data file");
    
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    date = fmt::format("{:%Y-%m-%dT%H:%M:%SZ}\n", *std::gmtime(&t));
    std::cout << "Local data updated at " << date << '\n';

    std::string dataStr;
    jsoncons::encode_json(*this, dataStr);

    dataFile << dataStr;
}

void Data::restore() {
    std::filesystem::path currentPath { std::filesystem::current_path() };

    if (!std::filesystem::exists(currentPath / "version") || !std::filesystem::exists(currentPath / "data.json"))
        return;

    std::ifstream versionFile(currentPath / "version");
    if (!versionFile.good())
        return;
    versionFile >> version;

    std::ifstream dataFile(currentPath / "data.json");
    if (!dataFile.good())
        return;
    *this = jsoncons::decode_json<Data>(dataFile);
}