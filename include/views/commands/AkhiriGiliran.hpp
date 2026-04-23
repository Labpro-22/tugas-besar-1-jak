#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// AKHIRI_GILIRAN
class AkhiriGiliranCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi akhiri giliran yang ada di kelas Game
        game.endTurn();
        return true; // True karena buat ganti giliran pemain
    }
    
    std::string getName() const override { 
        return "AKHIRI_GILIRAN"; 
    }
};