#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEPAS
class LepasCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.lepasLelang();
        return false;
    }
    std::string getName() const override { 
        return "LEPAS"; 
    }
};