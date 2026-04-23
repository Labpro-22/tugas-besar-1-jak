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

    bool execute(IGameAction& game) override {
        // Panggil fungsi tawar yang ada di kelas Game
        game.placeBid(amount);
        return false;
    }
    
    std::string getName() const override { 
        return "TAWAR"; 
    }
};