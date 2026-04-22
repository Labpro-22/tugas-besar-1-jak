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

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi gunakan kemampuan yang ada di kelas Game
        // Misal: game.gunakanKartuKemampuan(cardIndex);
    }
    
    std::string getName() const override { 
        return "GUNAKAN_KEMAMPUAN"; 
    }
};