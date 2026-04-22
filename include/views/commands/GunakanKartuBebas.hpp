#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// GUNAKAN_KARTU_BEBAS
class GunakanKartuBebasCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi beli yang ada di kelas Game
        // Misal: game.gunakanKartuBebas();
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KARTU_BEBAS"; 
    }
};