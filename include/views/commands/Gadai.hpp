#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include "models/Player.hpp"
#include "models/Board.hpp"
#include "models/PropertyTile.hpp"
#include <string>
#include <vector>

// GADAI
class GadaiCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        Player* player = game.getCurrentPlayer();
        if (!player) return false;

        std::vector<PropertyTile*> mortgageable;
        for (auto* prop : player->getOwnedProperties()) {
            if (!prop->isMortgaged()) mortgageable.push_back(prop);
        }

        if (mortgageable.empty()) {
            std::cout << "Tidak ada properti yang dapat digadaikan saat ini.\n";
            return false;
        }

        std::cout << "=== Properti yang Dapat Digadaikan ===\n";
        for (size_t i = 0; i < mortgageable.size(); ++i) {
            std::cout << i + 1 << ". " << mortgageable[i]->getName() << " (" << mortgageable[i]->getCode() 
                      << ") Nilai Gadai: M" << mortgageable[i]->getMortgageValue() << "\n";
        }
        std::cout << "\nPilih nomor properti (0 untuk batal): ";

        std::string input;
        std::getline(std::cin, input);
        int choice = 0;
        try { choice = std::stoi(input); } catch (...) { return false; }

        if (choice > 0 && choice <= (int)mortgageable.size()) {
            PropertyTile* target = mortgageable[choice - 1];
            StreetTile* st = dynamic_cast<StreetTile*>(target);

            // Cek apakah ada bangunan di color group ini
            if (st) {
                std::string color = st->getColorGroup();
                auto group = game.getBoard().getPropertiesByColor(color);
                bool hasBuildings = false;
                
                for (auto* p : group) {
                    StreetTile* s = dynamic_cast<StreetTile*>(p);
                    if (s && s->getBuildingLevel() > 0) { hasBuildings = true; break; }
                }

                if (hasBuildings) {
                    std::cout << "\n" << target->getName() << " tidak dapat digadaikan!\n";
                    std::cout << "Masih terdapat bangunan di color group [" << color << "].\n";
                    std::cout << "Bangunan harus dijual terlebih dahulu.\n\n";
                    
                    std::cout << "Daftar bangunan di color group [" << color << "]:\n";
                    for (auto* p : group) {
                        StreetTile* s = dynamic_cast<StreetTile*>(p);
                        if (s && s->getBuildingLevel() > 0) {
                            std::cout << "- " << s->getName() << " (" << s->getCode() << ") - " 
                                      << s->getBuildingLevel() << " rumah -> Nilai jual bangunan: M" 
                                      << s->getBuildingSaleValue() << "\n";
                        }
                    }
                    
                    std::cout << "\nJual semua bangunan color group [" << color << "]? (y/n): ";
                    std::getline(std::cin, input);
                    if (input == "y" || input == "Y") {
                        game.sellAllBuildingsInGroup(color); // Jual bangunan
                        std::cout << "\nLanjut menggadaikan " << target->getName() << "? (y/n): ";
                        std::getline(std::cin, input);
                        if (input != "y" && input != "Y") return false;
                    } else {
                        return false;
                    }
                }
            }
            // Lanjut Gadai
            game.mortgageProperty(target->getCode());
        }
        return false;
    }
    
    std::string getName() const override { 
        return "GADAI"; 
    }
};