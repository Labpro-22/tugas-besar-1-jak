#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// JUAL_BANGUNAN <kode_petak>
class JualBangunanCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit JualBangunanCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi jual bangunan yang ada di kelas Game
        game.buildOnProperty(tileCode);
        return false;
    }
    
    std::string getName() const override { 
        return "JUAL_BANGUNAN"; 
    }
};