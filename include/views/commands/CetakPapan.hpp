#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_PAPAN
class CetakPapanCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.cetakPapan();
        return false;
    }
    
    std::string getName() const override { 
        return "CETAK_PAPAN"; 
    }
};