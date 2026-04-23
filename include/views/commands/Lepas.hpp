#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEPAS
class LepasCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi lepas yang ada di kelas Game
        game.passAuction();
        return false;
    }
    std::string getName() const override { 
        return "LEPAS"; 
    }
};