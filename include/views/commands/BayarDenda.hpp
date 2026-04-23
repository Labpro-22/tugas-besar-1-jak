#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// BAYAR_DENDA
class BayarDendaCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi bayar denda yang ada di kelas Game
        game.payJailFine();
        return false; // Ga ngubah giliran
    }
    
    std::string getName() const override { 
        return "BAYAR_DENDA"; 
    }
};