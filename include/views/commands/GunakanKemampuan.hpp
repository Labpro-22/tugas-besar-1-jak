#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// GUNAKAN_KEMAMPUAN <indeks_kartu>
class GunakanKemampuanCommand : public Command {
    private:
    int cardIndex;
    
    public:
    explicit GunakanKemampuanCommand(int cardIndex) : cardIndex(cardIndex) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi gunakan kemampuan yang ada di kelas Game
        game.useSkillCard(cardIndex);
        return false;
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KEMAMPUAN"; 
    }
};