#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class BangkrutCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        // TODO: panggil fungsi bangkrut yang ada di kelas Game
        // Misal: game.bangkrut();
        return true; // True karena kalau udah bangkrut, gilirannya berganti
    }
    
    std::string getName() const override { 
        return "BANGKRUT"; 
    }
};