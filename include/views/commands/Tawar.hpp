#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// TAWAR <jumlah>
class TawarCommand : public Command {
    private:
    int amount;
    
    public:
    explicit TawarCommand(int amount) : amount(amount) {}

    bool execute(IGameAction& game) override {
        game.tawar(amount);
        return false;
    }
    
    std::string getName() const override { 
        return "TAWAR"; 
    }
};