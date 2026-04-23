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

    bool execute(IGameAction& game) override {
        game.muatGame(filename);
        return false;
    }
    
    std::string getName() const override { 
        return "MUAT"; 
    }
};
