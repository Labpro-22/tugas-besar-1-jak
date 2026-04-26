#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// BANGKRUT
class BangkrutCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi bangkrut yang ada di kelas Game
        game.declareBankruptcy();
        return true; // True karena kalau udah bangkrut, game selesai
    }
    
    std::string getName() const override { 
        return "BANGKRUT"; 
    }
};