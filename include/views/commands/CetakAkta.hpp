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

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi cetak akta yang ada di kelas Game
        // Misal: game.cetakAkta(tileCode);
    }

    std::string getName() const override { 
        return "CETAK_AKTA"; 
    }
};