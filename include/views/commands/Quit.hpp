#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class QuitCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // TODO: panggil fungsi keluar yang ada di kelas Game
        // Misal: game.quitGame();
        return true; // True karena game-nya berhenti, otomatis gilirannya habis
    }
    
    std::string getName() const override { 
        return "QUIT"; 
    }
};