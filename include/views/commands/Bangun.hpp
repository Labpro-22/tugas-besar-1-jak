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

    bool execute(IGameAction& game) override {
        game.bangunDiProperti(tileCode);
        return false;
    }
    
    std::string getName() const override { 
        return "BANGUN"; 
    }
};