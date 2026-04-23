#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// TEBUS <kode_petak>
class TebusCommand : public Command {
    private:
    std::string tileCode;
    
    public:
    explicit TebusCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi tebus yang ada di kelas Game
        game.redeemProperty(tileCode);
        return false;
    }
    
    std::string getName() const override { 
        return "TEBUS"; 
    }
};