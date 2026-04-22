#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_PROPERTI
class CetakPropertiCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        // TODO: panggil fungsi cetak properti yang ada di kelas Game
        // Misal: game.cetakInventoriProperti();
    }
    
    std::string getName() const override { 
        return "CETAK_PROPERTI"; 
    }
};