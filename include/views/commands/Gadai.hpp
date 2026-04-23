#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>
#include <vector>

// GADAI <kode_petak> [<kode_petak> ...]
class GadaiCommand : public Command {
    private:
    std::vector<std::string> tileCodes;

    public:
    explicit GadaiCommand(std::vector<std::string> tileCodes) : tileCodes(std::move(tileCodes)) {}

    bool execute(IGameAction& game) override {
        for (const auto& code : tileCodes) {
            // Panggil fungsi gadai yang ada di kelas Game
            game.mortgageProperty(code);
            return false;
        }
    }
    
    std::string getName() const override { 
        return "GADAI"; 
    }
};