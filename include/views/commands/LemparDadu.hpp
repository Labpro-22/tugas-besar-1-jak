#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEMPAR_DADU
class LemparDaduCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi kocok dadu yang ada di kelas Game
        // Misal: game.lemparDadu();
    }

    std::string getName() const override { 
        return "LEMPAR_DADU"; 
    }
};