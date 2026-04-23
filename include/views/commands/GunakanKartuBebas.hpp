#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// GUNAKAN_KARTU_BEBAS
class GunakanKartuBebasCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi beli yang ada di kelas Game
        game.useJailFreeCard();
        return false;
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KARTU_BEBAS"; 
    }
};