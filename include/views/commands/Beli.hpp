#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BELI
class BeliCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi beli yang ada di kelas Game
        game.buyCurrentProperty();
        return false; // Ga ngubah giliran
    }
    
    std::string getName() const override { 
        return "BELI"; 
    }
};