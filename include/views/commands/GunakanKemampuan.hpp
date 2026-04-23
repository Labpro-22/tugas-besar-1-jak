#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// GUNAKAN_KEMAMPUAN <indeks_kartu>
class GunakanKemampuanCommand : public Command {
    private:
    int cardIndex;
    
    public:
    explicit GunakanKemampuanCommand(int cardIndex) : cardIndex(cardIndex) {}

    bool execute(IGameAction& game) override {
        game.gunakanKartuKemampuan(cardIndex);
        return false;
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KEMAMPUAN"; 
    }
};