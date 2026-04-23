#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_AKTA [<kode_petak>]
class CetakAktaCommand : public Command {
    private:
    std::string tileCode;

    public:
    explicit CetakAktaCommand(std::string tileCode = "") : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi cetak akta yang ada di kelas Game
        game.printDeed(tileCode);
        return false;
    }

    std::string getName() const override { 
        return "CETAK_AKTA"; 
    }
};