#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// GUNAKAN_KARTU_BEBAS
class GunakanKartuBebasCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.gunakanKartuBebas();
        return false;
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KARTU_BEBAS"; 
    }
};