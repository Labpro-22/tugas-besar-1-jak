#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// BAYAR_DENDA
class BayarDendaCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi bayar denda yang ada di kelas Game
        // Misal: game.bayarDenda();
    }
    
    std::string getName() const override { 
        return "BAYAR_DENDA"; 
    }
};