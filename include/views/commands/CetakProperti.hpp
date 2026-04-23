#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_PROPERTI
class CetakPropertiCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.cetakInventoriProperti();
        return false;
    }
    
    std::string getName() const override { 
        return "CETAK_PROPERTI"; 
    }
};