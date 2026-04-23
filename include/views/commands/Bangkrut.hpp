#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class BangkrutCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        game.bangkrut();
        return true; // True karena kalau udah bangkrut, gilirannya berganti
    }
    
    std::string getName() const override { 
        return "BANGKRUT"; 
    }
};