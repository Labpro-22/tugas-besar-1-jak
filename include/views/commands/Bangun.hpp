#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BANGUN <kode_petak>
class BangunCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit BangunCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi bangun yang ada di kelas Game
        // Misal: game.bangunDiProperti(tileCode);
    }
    
    std::string getName() const override { 
        return "BANGUN"; 
    }
};