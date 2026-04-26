#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include "models/Player.hpp"
#include "models/Board.hpp"
#include "models/PropertyTile.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// BANGUN
class BangunCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit BangunCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        Player* player = game.getCurrentPlayer();
        if (!player) return false;

        // Satuin color group yang dimonopoli
        std::vector<std::string> eligibleGroups;
        for (auto* prop : player->getOwnedProperties()) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (!st || st->isMortgaged() || !st->isMonopolized()) continue;
            
            std::string color = st->getColorGroup();
            if (std::find(eligibleGroups.begin(), eligibleGroups.end(), color) == eligibleGroups.end()) {
                eligibleGroups.push_back(color);
            }
        }

        if (eligibleGroups.empty()) {
            std::cout << "Tidak ada color group yang memenuhi syarat untuk dibangun.\n";
            std::cout << "Kamu harus memiliki seluruh petak dalam satu color group terlebih dahulu.\n";
            return false;
        }

        // Tampilkan pilihan color group
        std::cout << "=== Color Group yang Memenuhi Syarat ===\n";
        for (size_t i = 0; i < eligibleGroups.size(); i++) {
            std::cout << i + 1 << ". [" << eligibleGroups[i] << "]\n";
            auto group = game.getBoard().getPropertiesByColor(eligibleGroups[i]);
            for (auto* prop : group) {
                StreetTile* st = dynamic_cast<StreetTile*>(prop);
                if (st) {
                    std::string level = st->getBuildingLevel() == 5 ? "Hotel" : std::to_string(st->getBuildingLevel()) + " rumah";
                    int cost = st->getBuildingLevel() == 4 ? st->getHotelCost() : st->getHouseCost();
                    std::cout << "   - " << st->getName() << " (" << st->getCode() << ") \t: " << level << " (Harga bangun: M" << cost << ")\n";
                }
            }
        }

        std::cout << "\nUang kamu saat ini : M" << player->getCash() << "\n";
        std::cout << "Pilih nomor color group (0 untuk batal): ";
        
        std::string input;
        std::getline(std::cin, input);
        int groupChoice = 0;
        try { groupChoice = std::stoi(input); } catch (...) { return false; }
        if (groupChoice <= 0 || groupChoice > (int)eligibleGroups.size()) return false;

        std::string chosenColor = eligibleGroups[groupChoice - 1];
        auto group = game.getBoard().getPropertiesByColor(chosenColor);

        // Tampilkan petak yang bisa dibangun (cek pemerataan)
        std::cout << "\nColor group [" << chosenColor << "]:\n";
        
        // Cari level bangunan paling rendah di group ini
        int minLevel = 5;
        for (auto* prop : group) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (st && st->getBuildingLevel() < minLevel) minLevel = st->getBuildingLevel();
        }

        std::vector<StreetTile*> buildable;
        for (auto* prop : group) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (!st) continue;
            
            std::string level = st->getBuildingLevel() == 5 ? "Hotel" : std::to_string(st->getBuildingLevel()) + " rumah";
            
            // Syarat bisa dibangun: belum hotel DAN levelnya sama dengan level terendah di grupnya (pemerataan)
            if (st->getBuildingLevel() < 5 && st->getBuildingLevel() == minLevel) {
                std::string status = (st->getBuildingLevel() == 4) ? " <- siap upgrade ke hotel" : " <- dapat dibangun";
                std::cout << "   - " << st->getName() << " (" << st->getCode() << ") \t: " << level << status << "\n";
                buildable.push_back(st);
            } else {
                std::string status = (st->getBuildingLevel() == 5) ? " <- sudah maksimal, tidak dapat dibangun" : "";
                std::cout << "   - " << st->getName() << " (" << st->getCode() << ") \t: " << level << status << "\n";
            }
        }

        if (buildable.empty()) {
            return false; // Semuanya udah hotel
        }

        std::cout << "Pilih petak (0 untuk batal): ";
        std::getline(std::cin, input);
        int tileChoice = 0;
        try { tileChoice = std::stoi(input); } catch (...) { return false; }
        if (tileChoice <= 0 || tileChoice > (int)buildable.size()) return false;

        StreetTile* chosen = buildable[tileChoice - 1];
        int cost = (chosen->getBuildingLevel() == 4) ? chosen->getHotelCost() : chosen->getHouseCost();

        if (player->getCash() < cost) {
            std::cout << "Uang tidak cukup. Butuh M" << cost << "\n";
            return false;
        }

        // Konfirmasi khusus Hotel
        if (chosen->getBuildingLevel() == 4) {
            std::cout << "Upgrade ke hotel? Biaya: M" << cost << " (y/n): ";
            std::getline(std::cin, input);
            if (input != "y" && input != "Y") return false;
        }

        // Panggil fungsi bangun yang ada di Game
        game.buildOnProperty(chosen->getCode());
        return false;
    }
    
    std::string getName() const override { 
        return "BANGUN"; 
    }
};