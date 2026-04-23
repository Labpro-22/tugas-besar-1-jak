#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BANTUAN
class BantuanCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi bantuan yang ada di kelas Game
        game.printHelp();
        return false;
    }

    std::string getName() const override { 
        return "BANTUAN"; 
    }
};