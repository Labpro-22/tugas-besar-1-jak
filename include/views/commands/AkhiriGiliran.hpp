#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class AkhiriGiliranCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // TODO: panggil fungsi akhiri giliran yang ada di kelas Game
        // Misal: game.akhiriGiliran();
        return true; // True karena buat ganti giliran pemain
    }
    
    std::string getName() const override { 
        return "AKHIRI_GILIRAN"; 
    }
};