#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include "models/Player.hpp"
#include "models/PropertyTile.hpp"
#include <string>

// TEBUS <kode_petak>
class TebusCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit TebusCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        Player* player = game.getCurrentPlayer();
        if (!player) return false;

        std::vector<PropertyTile*> mortgaged;
        for (auto* prop : player->getOwnedProperties()) {
            if (prop->isMortgaged()) mortgaged.push_back(prop);
        }

        if (mortgaged.empty()) {
            std::cout << "Tidak ada properti yang sedang digadaikan.\n";
            return false;
        }

        std::cout << "=== Properti yang Sedang Digadaikan ===\n";
        for (size_t i = 0; i < mortgaged.size(); ++i) {
            std::cout << i + 1 << ". " << mortgaged[i]->getName() << " (" << mortgaged[i]->getCode() 
                      << ") [M] Harga Tebus: M" << mortgaged[i]->getPrice() << "\n";
        }
        
        std::cout << "\nUang kamu saat ini: M" << player->getCash() << "\n";
        std::cout << "Pilih nomor properti (0 untuk batal): ";

        std::string input;
        std::getline(std::cin, input);
        int choice = 0;
        try { choice = std::stoi(input); } catch (...) { return false; }

        if (choice > 0 && choice <= (int)mortgaged.size()) {
            PropertyTile* target = mortgaged[choice - 1];
            if (player->getCash() < target->getPrice()) {
                std::cout << "Uang kamu tidak cukup untuk menebus " << target->getName() << ".\n";
                std::cout << "Harga tebus: M" << target->getPrice() << " | Uang kamu: M" << player->getCash() << "\n";
            } else {
                game.redeemProperty(target->getCode()); // Lempar ke Game
            }
        }
        return false;
    }
    
    std::string getName() const override { 
        return "TEBUS"; 
    }
};