#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// JUAL_BANGUNAN <kode_petak>
class JualBangunanCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit JualBangunanCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi jual bangunan yang ada di kelas Game
        // Misal: game.jualBangunan(tileCode);
    }
    
    std::string getName() const override { 
        return "JUAL_BANGUNAN"; 
    }
};