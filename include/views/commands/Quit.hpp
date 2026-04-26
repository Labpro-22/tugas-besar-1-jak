#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// QUIT
class QuitCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi keluar yang ada di kelas Game
        game.setGameActive(false);
        return true; // True karena ngeberhentiin game
    }
    
    std::string getName() const override { 
        return "QUIT"; 
    }
};