#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// CETAK_PROPERTI
class CetakPropertiCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi cetak properti yang ada di kelas Game
        game.printPropertyInventory();
        return false;
    }
    
    std::string getName() const override { 
        return "CETAK_PROPERTI"; 
    }
};