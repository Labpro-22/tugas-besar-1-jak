#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// CETAK_STATUS
class CetakStatusommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi cetak status yang ada di kelas Game
        game.printPlayerStatus();
        return false;
    }

    std::string getName() const override { 
        return "BANTUAN"; 
    }
};