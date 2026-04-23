#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class QuitCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        game.quitGame();
        return true; // True karena game-nya berhenti, otomatis gilirannya habis
    }
    
    std::string getName() const override { 
        return "QUIT"; 
    }
};