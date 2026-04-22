#pragma once
#include <vector>
#include <string>
#include <map>
#include "Tile.hpp"

class ConfigLoader
{
public:
    // Ngeload papan permainan
    static std::vector<Tile *> loadBoard(const std::string &folderPath = "config/");

    // Ngeload konfigurasi angka (misc, tax, sama special)
    static std::map<std::string, int> loadTax(const std::string &folderPath = "config/");
    static std::map<std::string, int> loadSpecial(const std::string &folderPath = "config/");
    static std::map<std::string, int> loadMisc(const std::string &folderPath = "config/");

    // Ngeload tabel sewa (key: jumlah dimiliki, value: harga/pengali)
    static std::map<int, int> loadRailroad(const std::string &folderPath = "config/");
    static std::map<int, int> loadUtility(const std::string &folderPath = "config/");
};