#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// CETAK_PAPAN
class CetakPapanCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi cetak/print yang ada di kelas Game
        game.printBoard();
        return false;
    }
    
    std::string getName() const override { 
        return "CETAK_PAPAN"; 
    }
};