#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BELI
class BeliCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi beli yang ada di kelas Game
        // Misal: game.beliProperti();
    }
    
    std::string getName() const override { 
        return "BELI"; 
    }
};