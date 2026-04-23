#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// BANGUN <kode_petak>
class BangunCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit BangunCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi bangun yang ada di kelas Game
        game.buildOnProperty(tileCode);
        return false; // Ga ngubah giliran
    }
    
    std::string getName() const override { 
        return "BANGUN"; 
    }
};