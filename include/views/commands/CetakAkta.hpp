#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>
#include <iostream>

// CETAK_AKTA [<kode_petak>]
class CetakAktaCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Tampilin daftar propertinya
        game.printPropertyInventory();
        
        // Dari sini yang minta input
        std::cout << "Masukkan kode petak: ";
        std::string tileCode;
        std::getline(std::cin, tileCode);

        // Samain ke huruf kapital
        for (char &c : tileCode) {
            c = std::toupper(c);
        }

        // Panggil fungsi cetak akta yang ada di Game
        game.printDeed(tileCode);
        
        return false;
    }

    std::string getName() const override { 
        return "CETAK_AKTA"; 
    }
};