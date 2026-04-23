#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BELI
class BeliCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.beliProperti();
        return false;
    }
    
    std::string getName() const override { 
        return "BELI"; 
    }
};