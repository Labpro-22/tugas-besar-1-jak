#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEMPAR_DADU
class LemparDaduCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi kocok dadu yang ada di kelas Game
        game.rollDice();
        return false;
    }

    std::string getName() const override { 
        return "LEMPAR_DADU"; 
    }
};