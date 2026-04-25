#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>
#include <vector>

// GADAI <kode_petak> [<kode_petak> ...]
class GadaiCommand : public Command {
    public:
    bool execute(IGameAction& game) override {
        // Panggil fungsi gadai yang ada di kelas Game
        game.mortgageProperty();
        return false;
    }
    
    std::string getName() const override { 
        return "GADAI"; 
    }
};