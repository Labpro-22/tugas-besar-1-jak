#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// TAWAR <jumlah>
class TawarCommand : public Command {
    private:
    int amount;
    
    public:
    explicit TawarCommand(int amount) : amount(amount) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi tawar yang ada di kelas Game
        // Misal: game.tawar(amount);
    }
    
    std::string getName() const override { 
        return "TAWAR"; 
    }
};