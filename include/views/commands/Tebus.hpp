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

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi tebus yang ada di kelas Game
        // Misal: game.tebusProperti(tileCode);
    }
    
    std::string getName() const override { 
        return "TEBUS"; 
    }
};