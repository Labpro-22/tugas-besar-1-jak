#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// CETAK_AKTA [<kode_petak>]
class CetakAktaCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi cetak akta yang ada di kelas Game
        game.printDeed();
        return false;
    }

    std::string getName() const override { 
        return "CETAK_AKTA"; 
    }
};