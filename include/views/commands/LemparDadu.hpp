#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEMPAR_DADU
class LemparDaduCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        game.lemparDadu();
        return false;
    }

    std::string getName() const override { 
        return "LEMPAR_DADU"; 
    }
};