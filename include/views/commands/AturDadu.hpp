#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// ATUR_DADU X Y
class AturDaduCommand : public Command {
    private:
    int x, y;

    public:
    AturDaduCommand(int x, int y) : x(x), y(y) {}

    bool execute(IGameAction& game) override {
        game.aturDadu(x, y);
        return false;
    }
    
    std::string getName() const override { 
        return "ATUR_DADU"; 
    }
};