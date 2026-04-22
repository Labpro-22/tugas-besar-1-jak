#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// MUAT <filename>
class MuatCommand : public Command {
    private:
    std::string filename;
    
    public:
    explicit MuatCommand(std::string filename) : filename(std::move(filename)) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi muat game yang ada di kelas Game
        // Misal: game.muatGame(filename);
    }
    
    std::string getName() const override { 
        return "MUAT"; 
    }
};
