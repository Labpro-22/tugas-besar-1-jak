#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>
#include <vector>

// GADAI <kode_petak> [<kode_petak> ...]
class GadaiCommand : public Command {
    private:
    std::vector<string> tileCodes;

    public:
    explicit GadaiCommand(std::vector<string> tileCodes) : tileCodes(std::move(tileCodes)) {}

    void execute(IGameAction& game) override {
        for (const auto& code : tileCodes) {
            // TODO: panggil fungsi gadai yang ada di kelas Game
            // Misal: game.gadaiProperti(code);
        }
    }
    
    std::string getName() const override { 
        return "GADAI"; 
    }
};