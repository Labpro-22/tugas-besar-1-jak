#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// AKHIRI_GILIRAN
class AkhiriGiliranCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi akhiri giliran yang ada di kelas Game
        game.endTurn();
        return false; // True karena buat ganti giliran pemain
    }
    
    std::string getName() const override { 
        return "AKHIRI_GILIRAN"; 
    }
};