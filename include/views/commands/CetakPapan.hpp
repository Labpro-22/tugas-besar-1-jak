#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_PAPAN
class CetakPapanCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi cetak/print yang ada di kelas Game
        // Misal: game.cetakPapan();
    }
    
    std::string getName() const override { 
        return "CETAK_PAPAN"; 
    }
};