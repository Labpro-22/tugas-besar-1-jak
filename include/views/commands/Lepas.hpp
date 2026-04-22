#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// LEPAS
class LepasCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi lepas yang ada di kelas Game
        // Misal: game.lepasLelang();
    }
    std::string getName() const override { 
        return "LEPAS"; 
    }
};