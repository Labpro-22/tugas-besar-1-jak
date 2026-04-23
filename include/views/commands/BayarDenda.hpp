#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BAYAR_DENDA
class BayarDendaCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.bayarDenda();
        return false;
    }
    
    std::string getName() const override { 
        return "BAYAR_DENDA"; 
    }
};